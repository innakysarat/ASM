format PE console
entry start

include 'win32a.inc'

;     +oo               +oo
;     \--    1          \--    1
; e =  >    ---  =  1 +  >    ---
;     /--    n!         /--    n!
;     n = 0            n = 1


;-------------------------------first act - variables--------------------------
section '.data' data readable writable

    fmt_step_output db "step %d: n = %d, n! = %d, sum = %.10f", 13, 10, 0
    fmt_full_output db "-----------------------------------------------", 13, 10,\
                       "e = %f", 13, 10,\
                       "-----------------------------------------------", 13, 10, 0

    sum dq 1.000 ; we skipped first element in sum, which is 1
    prev dq 0.000 ; sum on previous step
    acc dq 0.001 ; accuracy (0.1%)

    n dd 0 ; loop counter
    fact dd 1 ; fact for current loop step

    tmp_esp dd ? ; used for saving stack ptr in SumLoop


;-------------------------------second act - script--------------------------
section '.code' code readable executable

start:

    FINIT ; init FPU api

    call SumLoop ; count sum

    invoke printf, fmt_full_output, dword[sum], dword[sum+4] ; print sum

    invoke getch
    invoke ExitProcess, 0 ; end programm


;-------------------------------     procedures    ---------------------------

; Count next sum approximation, until it pass accurancy check
SumLoop:
    mov [tmp_esp], esp ; Because printf use stack

acc_loop:
        FLD [sum]
        FSTP [prev] ; Save sum to prev via FPU stack
        WAIT


        call NextSumApproximation ; Count next e approximation

        invoke printf, fmt_step_output, [n], [n], [fact], dword[sum], dword[sum+4] ; Print it

        call CheckAccuracy ; Check if it fits approximation requirments
        ja acc_loop ; if no - run loop again

    mov esp, [tmp_esp]
    ret

; Count next sum approximation
NextSumApproximation:

    mov eax, [n]
    inc eax
    mov [n], eax ; eax = ++n

    mul [fact]
    mov [fact], eax ; fact *= n

    FLD1
    FIDIV [fact]
    FADD [sum]
    FSTP [sum] ; sum = sum + (1 / fact)
    WAIT

    ret

; Check, whether difference between sum and prev_sum is less then acc
CheckAccuracy:
    FLD [sum]
    FSUB [prev]
    FABS ; st(0) = abs(sum - prev)
    FDIV [sum] ; st(0) = abs(sum - prev) / sum

    FCOMP [acc]
    FSTSW ax ; FPU flags to ax
    WAIT

    sahf ; ax to CPU flags

    ret




;-------------------------------third act - includes--------------------------
section '.idata' import data readable
    library kernel, 'kernel32.dll',\
            msvcrt, 'msvcrt.dll',\
            user32,'USER32.DLL'

include 'api\user32.inc'
include 'api\kernel32.inc'
    import kernel,\
           ExitProcess, 'ExitProcess',\
           HeapCreate,'HeapCreate',\
           HeapAlloc,'HeapAlloc'
    include 'api\kernel32.inc'
    import msvcrt,\
           printf, 'printf',\
           sprintf, 'sprintf',\
           scanf, 'scanf',\
           getch, '_getch'
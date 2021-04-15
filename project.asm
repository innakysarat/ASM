format PE console
entry start

include 'win32a.inc'

 ;th(x) = (e^(2x) -1) /(e^(2x)+1)


;-------------------------------first act - variables--------------------------
section '.data' data readable writable


    fmt_full_output db "-----------------------------------------------", 13, 10,\
                       "th(x) = %f", 13, 10,\
                       "-----------------------------------------------", 13, 10, 0
    strX     db 'Input x: ', 0

    endBelow db "-----------------------------------------------", 13, 10,\
                "th(x) = -1.000000",13,10,\
                "-----------------------------------------------", 13, 10, 0
    endAbove db "-----------------------------------------------", 13, 10,\
                "th(x) = 1.000000",13,10,\
                "-----------------------------------------------", 13, 10, 0
    strScan  db '%lf', 0
    sum dq 1.000 ; skip first element in sum(= 1)
    prev dq 0.000 ; sum on previous step
    acc dq 0.001 ; accuracy (0.1%)

    buf rd 100
    x dq 0
    numerator dq 1.000
    two dq 2.000
    denominator dq ?
    counter dd 0 ; loop counter
    factorial dq 1.000 ; factorial for current loop step
    borderAbove dq 3.800
    borderUnder dq -3.800
    tmp_esp dd ? ; used for saving stack ptr in Sum


;-------------------------------second act - script--------------------------
section '.code' code readable executable

start:

    FINIT ; init FPU api

    call InputX

    call Sum ; count sum
    call Result
    invoke printf, fmt_full_output, dword[sum], dword[sum+4] ; print sum

    invoke getch
    invoke ExitProcess, 0 ; end program


;-------------------------------     procedures    ---------------------------
; Processing the entered value
InputX:

        push strX
        call [printf]
        add esp, 4

        invoke gets, buf
        invoke sscanf, buf, strScan, x
        add esp, 16

        cmp eax, 1
        jne InputX

        FLD [x]
        FCOMP [borderAbove]
        FSTSW ax ; FPU flags to ax
        WAIT

        sahf ; ax to CPU flags
        jb notAbove
           invoke printf, endAbove
           invoke getch
           invoke ExitProcess, 0

notAbove:
        FLD [x]
        FCOMP [borderUnder]
        FSTSW ax ; FPU flags to ax
        WAIT

        sahf ; ax to CPU flags
        ja notBelow
           invoke printf, endBelow
           invoke getch
           invoke ExitProcess, 0

notBelow:

        ret


; Count next sum, until it passes accurancy check
Sum:
    mov [tmp_esp], esp ; printf uses stack so we should save esp
    FLD [x]
    FMUL [two]
    FSTP [x]

acc_loop:
        FLD [sum]
        FSTP [prev] ; Save sum to prev via FPU stack
        WAIT


        call NextSum ; Count next sum approximation

        call CheckAccuracy ; Check if it fits approximation requirements
        ja acc_loop ; in case it doesn't fit - run loop again

    mov esp, [tmp_esp]
    ret


; count total th(x) value
Result:

    FLD1
    FADD [sum]
    FSTP[denominator]  ; calculating the denominator of th(x)

    FLD1
    FSUBR[sum] ; calculating the numerator of th(x)

    FDIV[denominator]
    FSTP[sum] ;total th(x) value

    WAIT

    ret
; Count next sum approximation

NextSum:

    FLD[numerator]
    FMUL[x]          ; numerator *= x;
    FSTP[numerator]

    WAIT

    mov eax, [counter]
    inc eax
    mov [counter], eax ; eax = ++n


    FLD [factorial]
    FIMUL [counter]    ; factorial = counter!
    FSTP [factorial]


    FLD [numerator]
    FDIV [factorial]
    FADD [sum]
    FSTP [sum] ; sum = sum + (x / fact)


    WAIT

    ret

; Check, whether difference between sum and prev_sum is less then acc
CheckAccuracy:
    FLD [sum]
    FSUB [prev]
    FABS ; st(0) = abs(sum - prev)
    FDIV [sum] ; st(0) = abs(sum - prev) / sum

    FABS     ; abs((sum-prev)/ sum)
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
           sscanf, 'sscanf',\
           scanf, 'scanf',\
           puts, 'puts',\
           getchar, 'getchar',\
           fflush, 'fflush',\
           gets, 'gets',\
           getch, '_getch'
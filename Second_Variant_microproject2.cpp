#include <iostream>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h> 
#include <stdlib.h>  
#include <stdio.h>  
#include <cstdlib>
#define NOMINMAX
#ifdef _WIN32
#include <conio.h>
#include <Windows.h>
#else
#include <unistd.h>
#endif

static int count = 0;
size_t n;
size_t H;
int over = 0;
pthread_t* bees;
int* pr;
pthread_mutex_t mutex;
pthread_cond_t not_full;

pthread_cond_t not_empty;

int input() {
    int n;
    std::cin >> n;
    bool test = true;
    do {
        if (!(test = std::cin.good())) {
            std::cout << "Incorrect input. Try again." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    } while (!test);
    return n;
}
void* Bear(void* param) {
    do {
        pthread_mutex_lock(&mutex);
        while (count != H)
            pthread_cond_wait(&not_full, &mutex);
        count = 0;
        printf("Bear ate\n");
        over++;
        pthread_mutex_unlock(&mutex);
        pthread_cond_broadcast(&not_empty);
        Sleep(1000);
    } while (over <= 5);
    return NULL;
}
void* Producer(void* param) {
    int pNum = *((int*)param);
    int i;
    while (1) {
        pthread_mutex_lock(&mutex);
        if (count == H) {
            pthread_cond_signal(&not_full);
            do {
                pthread_cond_wait(&not_empty, &mutex);
            } while (count == H);
        }
        count++;
        printf("Bee %d fills the pot\n", pNum);
        pthread_mutex_unlock(&mutex);
        Sleep(1000);
    }
    return NULL;
}

int main() {

    int i;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&not_full, NULL);
    pthread_cond_init(&not_empty, NULL);
    do {
        std::cout << "Input a number of bees <= 30:" << std::endl;
        n = input();
    } while (n <= 1 || n > 30);
    do {
        std::cout << "Input a number of sips: <= 25:" << std::endl;
        H = input();
    } while (H <= 0 || H > 25);
    bees = new pthread_t[n];
    pr = new int[n];
    for (i = 0; i < n; i++) {
        pr[i] = i + 1;
        pthread_create(&bees[i], NULL, Producer, (void*)(pr + i));
    }

    pthread_t c_thread;
    pthread_create(&c_thread, NULL, Bear, NULL);
    int mNum = 0;
    Bear((void*)&mNum);
    delete[] bees;
    delete[] pr;
    return 0;
}


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

int count = 0;
size_t n;
size_t H;
static int over = 0;
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
	while (over <= 5) {
		pthread_mutex_lock(&mutex);
		while (count != H)
			pthread_cond_wait(&not_full, &mutex);
		count = 0;
		pthread_mutex_unlock(&mutex);
		pthread_cond_broadcast(&not_full);
		printf("Bear ate\n");
		Sleep(1000);
		over++;
	}
	return NULL;
}
void* Producer(void* param) {
	int pNum = *((int*)param);
	int i;
	while (1) {

		pthread_mutex_lock(&mutex);

		while (count == H) {
			pthread_cond_wait(&not_empty, &mutex);
		}
		count++;
		pthread_mutex_unlock(&mutex);
		pthread_cond_signal(&not_empty);
		printf("Bee %d fills the pot\n", pNum);
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
		std::cout << "Input a number of sips less than the number of bees: <= 25:" << std::endl;
		H = input();
	} while (H <= 0 || H > 25 || H >= n);
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
	std::cout << "Bear ate";
	delete[] bees;
	delete[] pr;
	return 0;
}



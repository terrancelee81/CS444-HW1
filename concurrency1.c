/* Concurrency 1: The Producer-Consumer Problem
 * Group: 10-11
 * CS444
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "mt.h"

/* Buffer can hold up to 32 items */
#define BUFF_MAX 32

/* Item has a number for printing and a number for sleeping */
struct item {
	int print_num;
	int sleep_time;
};

/* Buffer, buffer count, and buffer mutex */
struct item buffer[BUFF_MAX];
int buff_count = 0;
pthread_mutex_t buff_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Functions for manipulating buffer */
void buff_push(struct item buff_item);
struct item buff_pop(void);
int buff_empty(void);
int buff_full(void);
void buff_lock(void);
void buff_unlock(void);

/* Consumer and producer functions */
void *consumer(void *arg);
void *producer(void *arg);

/* Functions for getting a random number */
int get_rand(void);
int get_rdrand(void);

int main(int argc, char **argv)
{
	/* Seed Mersenne Twister */
	unsigned long init[4] = {0x123, 0x234, 0x345, 0x456};
	unsigned long length = 4;
	init_by_array(init, length);
	
	/* Number of threads specified by user */
	int num_threads = atoi(argv[1]);
	pthread_t con_threads[num_threads];
	pthread_t prod_threads[num_threads];
	
	while(1) {		
		/* Create threads */
		int i;
		for (i = 0; i < num_threads; i++) {
			pthread_create(&(con_threads[i]), NULL, producer, NULL);
			pthread_create(&(prod_threads[i]), NULL, consumer, NULL);
		}
		
		/* Join threads */
		for (i = 0; i < num_threads; i++) {
			pthread_join(prod_threads[i], NULL);
			pthread_join(con_threads[i], NULL);
		}
	}
	
	return 0;
}

void buff_push(struct item buff_item)
{
	buffer[buff_count++] = buff_item;
}

struct item buff_pop(void)
{
	return buffer[--buff_count];
}

int buff_empty(void)
{
	return buff_count <= 0;
}

int buff_full(void)
{
	return buff_count >= 32;
}

void buff_lock(void)
{
	pthread_mutex_lock(&buff_mutex);
}

void buff_unlock(void)
{
	pthread_mutex_unlock(&buff_mutex);
}

void *consumer(void *arg)
{
	buff_lock();
	
	if (buff_empty()) {
		buff_unlock();
	}
	
	struct item buff_item = buff_pop();
	sleep(buff_item.sleep_time);
	printf("%d\n", buff_item.print_num);
	fflush(stdout);
	
	buff_unlock();
}

void *producer(void *arg)
{
	struct item buff_item;
	
	buff_lock();
	
	if (buff_full()) {
		buff_unlock();
	}
	
	sleep(get_rand() % 5 + 3);
	buff_item.print_num = get_rand() % 1000;
	buff_item.sleep_time = get_rand() % 8 + 2;
	buff_push(buff_item);
	
	buff_unlock();
}

int get_rand(void)
{
	int rand_num;
	
	unsigned int eax;
	unsigned int ecx;
	
	eax = 0x01;

	__asm__ __volatile__(
			"cpuid;"
			: "=c"(ecx)
			: "a"(eax)
			);
	
	if(ecx & 0x40000000)
		rand_num = get_rdrand();
	else
		rand_num = genrand_int32();
	
	return abs(rand_num);
}

int get_rdrand(void)
{
	unsigned int rand_num;
	
	__asm__ __volatile__(
			"rdrand %0"
			: "=r"(rand_num)
			);
						
	return rand_num;
}

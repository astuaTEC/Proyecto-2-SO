#ifndef CETHREADS
#define CETHREADS

#include <stdio.h>

#define MAX_THREADS 10
#define THREAD_STACK 1024 * 1024

#define NOERROR 0
#define MAXTHREADS 1
#define MALLOCERROR 2
#define CLONEERROR 3
#define INTHREAD 4
#define SIGNALERROR 5

/* Function Declarations: */

/* Initializes all threads white status inactive */
void init_threads();

/* Change from parent to child thread */
void CEthread_yield();

/* Verify status of cethread, free memory */
void CEthread_start(void (*func)(void));

/* Verify status of cethread, free memory */
void CEthread_start_A(void (*func)(int, int), int arg1, int arg2);

/* create a new thread */
int CEthread_create(void (*func)(void), int argc, int arg1, int arg2);

#endif
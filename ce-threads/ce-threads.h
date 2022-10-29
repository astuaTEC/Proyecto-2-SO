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

/* Change from parent to child thread, give CPU pocession to othre user level threads voluntarily */
void CEthread_yield();

/* Verify status of cethread, free memory */
void CEthread_start(void (*func)(void));

/* Verify status of cethread, free memory */
void CEthread_start_A(void (*func)(int, int), int arg1, int arg2);

/* Create a new thread */
int CEthread_create(void (*func)(void), int argc, int arg1, int arg2);

/* Wait for thread termination */
int CEthread_join(int id);

/* Terminate a thread */
int CEthread_end();

/* Wait for thread termination */
int CEthread_wait();

/* Pause single thread */
void CEthread_pause(double pause);


#endif
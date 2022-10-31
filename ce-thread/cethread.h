#ifndef CETHREAD_T_H
#define CETHREAD_T_H

#define _GNU_SOURCE

/* To use Linux pthread Library in Benchmark, you have to comment the USE_MYTHREAD macro */
#define USE_CETHREAD 1

/* include lib header files that you need here: */
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/time.h>
#include <stdatomic.h>
#include <string.h>
#include <pthread.h>

#define QUANTUM 15000

typedef unsigned int cethread_t;

typedef enum
{
    run,
    block,
    done,
    destroy
} cethread_status;

typedef struct
{
    cethread_t threadID;
    ucontext_t threadContext;
    cethread_status threadStatus;
    int elapsedTime;
    cethread_t blockingThread;
    void **valuePtr;
    void *returnVal;
} tcb;

typedef struct threadControlList
{
    tcb *thread;
    struct threadControlList *next;
} threadControlList;

typedef struct cemutex_t
{
    int lock;
    threadControlList *waitList;
} cemutex_t;

/* Function Declarations: */
void addThreadToTCB(tcb *item);
tcb *getNextJob();
void unblockThread(cethread_t tid);
tcb *getTCB(cethread_t tid);
void freeNode(threadControlList *t);
void destroyAll();
void freeThreadQueue();
static void sched_stcf();
tcb *create_tcb(cethread_t tid, bool createContext);
void setupAction();
void setupTimer();
void createMainThread();

/* create a new thread */
int cethread_create(cethread_t *thread, pthread_attr_t *attr, void *(*function)(void *), void *arg);

/* give CPU pocession to other user level threads voluntarily */
int cethread_yield();

/* terminate a thread */
void cethread_exit(void *value_ptr);

/* wait for thread termination */
int cethread_join(cethread_t thread, void **value_ptr);

/* initial the mutex lock */
int cemutex_init(cemutex_t *mutex, const pthread_mutexattr_t *mutexattr);

/* aquire the mutex lock */
int cemutex_lock(cemutex_t *mutex);

/* release the mutex lock */
int cemutex_unlock(cemutex_t *mutex);

/* destroy the mutex */
int cemutex_destroy(cemutex_t *mutex);

#ifdef USE_CETHREAD
#define pthread_t cethread_t
#define pthread_mutex_t cemutex_t
#define pthread_create cethread_create
#define pthread_exit cethread_exit
#define pthread_join cethread_join
#define pthread_mutex_init cemutex_init
#define pthread_mutex_lock cemutex_lock
#define pthread_mutex_unlock cemutex_unlock
#define pthread_mutex_destroy cemutex_destroy
#endif

#endif
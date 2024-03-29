#include "cethread.h"

threadControlList *allThreadControlBlocks = NULL;
static int threadIDs = 0;
static tcb *currentlyRunningThreadBlock;

void addThreadToTCB(tcb *item)
{
    threadControlList *temp = malloc(sizeof(threadControlList));
    temp->next = NULL;
    temp->thread = item;
    temp->thread->elapsedTime = item->elapsedTime;

    if (allThreadControlBlocks == NULL)
    {
        allThreadControlBlocks = temp;
        return;
    }

    int elapsedTime = item->elapsedTime;
    threadControlList *prev = allThreadControlBlocks;
    threadControlList *ptr = allThreadControlBlocks->next;
    while (ptr != NULL)
    {
        if (elapsedTime < ptr->thread->elapsedTime)
        {
            if (prev == allThreadControlBlocks)
            {
                temp->next = allThreadControlBlocks;
                allThreadControlBlocks = temp;
            }
            else
            {
                temp->next = ptr;
                prev->next = temp;
            }
            return;
        }
        prev = ptr;
        ptr = ptr->next;
    }

    prev->next = temp;
}

tcb *getNextJob()
{
    if (allThreadControlBlocks == NULL)
        return NULL;
    threadControlList *prev = NULL;
    threadControlList *ptr = allThreadControlBlocks;
    while (ptr != NULL)
    {
        if (ptr->thread->threadStatus == run)
        {
            if (prev == NULL)
                allThreadControlBlocks = allThreadControlBlocks->next;
            else
                prev->next = ptr->next;

            tcb *nextJob = ptr->thread;
            free(ptr);
            return nextJob;
        }
        prev = ptr;
        ptr = ptr->next;
    }
    // no job found
    return NULL;
}

void unblockThread(cethread_t tid)
{
    threadControlList *temp = allThreadControlBlocks;
    while (temp != NULL)
    {
        if (temp->thread->blockingThread == tid && temp->thread->threadStatus == block)
        {
            temp->thread->blockingThread = -1;
            temp->thread->threadStatus = run;
        }
        temp = temp->next;
    }
}

tcb *getTCB(cethread_t tid)
{
    threadControlList *temp = allThreadControlBlocks;
    while (temp != NULL)
    {
        if (temp->thread->threadID == tid)
            return temp->thread;
        temp = temp->next;
    }
    return NULL;
}

void freeNode(threadControlList *t)
{
    if (t == NULL)
        return;
    free(t->thread->threadContext.uc_stack.ss_sp);
    free(t->thread);
    free(t);
}

void destroyAll()
{
    threadControlList *prev = NULL;
    threadControlList *cur = allThreadControlBlocks;
    while (cur != NULL)
    {
        if (cur->thread->threadStatus == destroy)
        {
            prev->next = cur->next;
            cur = prev->next;
            freeNode(prev);
            continue;
        }
        prev = cur;
        cur = cur->next;
    }
}

tcb *create_tcb(cethread_t tid, bool createContext)
{
    tcb *thread = malloc(sizeof(tcb));
    thread->threadID = tid;
    thread->blockingThread = -1;
    thread->threadStatus = run;
    thread->elapsedTime = 0;
    thread->valuePtr = NULL;
    thread->returnVal = NULL;
    if (createContext)
    {
        getcontext(&(thread->threadContext));
        thread->threadContext.uc_link = NULL;
        thread->threadContext.uc_stack.ss_sp = malloc(SIGSTKSZ);
        thread->threadContext.uc_stack.ss_size = SIGSTKSZ;
    }
    return thread;
}

void setupAction()
{
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = &sched_sjf;
    sigaction(SIGPROF, &action, NULL);
}

void setupTimer()
{
    struct itimerval timer;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = QUANTUM;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = QUANTUM;
    setitimer(ITIMER_PROF, &timer, NULL);
}

void createMainThread()
{
    currentlyRunningThreadBlock = create_tcb(threadIDs++, false);
    setupAction();
    setupTimer();
    getcontext(&(currentlyRunningThreadBlock->threadContext));
}

/* create a new thread */
int cethread_create(cethread_t *thread, pthread_attr_t *attr, void *(*function)(void *), void *arg)
{
    printf("\033[0;31m");
    printf("===============> STATUS: cethread_create started\n");
    printf("\033[0;m");
    if (threadIDs == 0)
        createMainThread();
    tcb *threadBlock = create_tcb(threadIDs++, true);
    *thread = threadBlock->threadID;
    addThreadToTCB(threadBlock);
    makecontext(&(threadBlock->threadContext), (void *)function, 1, arg);
    return 0;
};

/* give CPU possession to other user-level threads voluntarily */
int cethread_yield()
{
    printf("\033[0;31m");
    printf("===============> STATUS: cethread_yield started\n");
    printf("\033[0;m");
    sched_sjf();
    return 0;
};

/* terminate a thread */
void cethread_exit(void *value_ptr)
{
    printf("\033[0;31m");
    printf("===============> STATUS: cethread_exit started\n");
    printf("\033[0;m");

    currentlyRunningThreadBlock->threadStatus = done;
    if (currentlyRunningThreadBlock->valuePtr != NULL)
    {
        *currentlyRunningThreadBlock->valuePtr = value_ptr;
        currentlyRunningThreadBlock->threadStatus = destroy;
    }
    else
        currentlyRunningThreadBlock->returnVal = value_ptr;
    unblockThread(currentlyRunningThreadBlock->threadID);
    sched_sjf();
};

/* Wait for thread termination */
int cethread_join(cethread_t thread, void **value_ptr)
{
    printf("\033[0;31m");
    printf("===============> STATUS: cethread_join started\n");
    printf("\033[0;m");

    tcb *threadToJoin = getTCB(thread);
    if (threadToJoin == NULL)
        return 0;

    if (threadToJoin->threadStatus == done)
    {
        if (value_ptr != NULL)
        {
            threadToJoin->threadStatus = destroy;
            *value_ptr = threadToJoin->returnVal;
        }
    }
    else
    {
        currentlyRunningThreadBlock->threadStatus = block;
        currentlyRunningThreadBlock->blockingThread = thread;

        threadToJoin->valuePtr = value_ptr;
        sched_sjf();
    }

    return 0;
};

/* initialize the mutex lock */
int cemutex_init(cemutex_t *mutex, const pthread_mutexattr_t *mutexattr)
{
    printf("\033[0;31m");
    printf("===============> STATUS: cemutex_init started\n");
    printf("\033[0;m");

    // initialize data structures for this mutex
    if (threadIDs == 0)
        createMainThread();
    mutex->waitList = NULL;
    mutex->lock = 0;
    return 0;
};

/* aquire the mutex lock */
int cemutex_lock(cemutex_t *mutex)
{
    printf("\033[0;31m");
    printf("===============> STATUS: cemutex_lock started\n");
    printf("\033[0;m");

    while (atomic_flag_test_and_set(&(mutex->lock)))
    {
        currentlyRunningThreadBlock->threadStatus = block;
        threadControlList *temp = malloc(sizeof(threadControlList));
        temp->next = mutex->waitList;
        temp->thread = currentlyRunningThreadBlock;
        mutex->waitList = temp;
        sched_sjf();
    }
    return 0;
};

/* release the mutex lock */
int cemutex_unlock(cemutex_t *mutex)
{
    printf("\033[0;31m");
    printf("===============> STATUS: cemutex_unlock started\n");
    printf("\033[0;m");

    threadControlList *cur = mutex->waitList;
    while (cur != NULL)
    {
        cur->thread->threadStatus = run;
        threadControlList *temp = cur;
        cur = cur->next;
        free(temp);
    }

    mutex->waitList = NULL;
    mutex->lock = 0;

    return 0;
};

/* destroy the mutex */
int cemutex_destroy(cemutex_t *mutex)
{
    printf("\033[0;31m");
    printf("===============> STATUS: cemutex_destroy started\n");
    printf("\033[0;m");

    cemutex_unlock(mutex);
    return 0;
};

/* Preemptive SJF (sjf) scheduling algorithm */
static void sched_sjf()
{
    destroyAll();
    signal(SIGPROF, SIG_IGN);

    tcb *prevThread = currentlyRunningThreadBlock;
    currentlyRunningThreadBlock = getNextJob();
    if (currentlyRunningThreadBlock != NULL)
    {
        prevThread->elapsedTime++;
        addThreadToTCB(prevThread);
        setupAction();
        setupTimer();
        swapcontext(&(prevThread->threadContext), &(currentlyRunningThreadBlock->threadContext));
    }
    else
    {
        currentlyRunningThreadBlock = prevThread;
    }
}
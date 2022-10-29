#include <ucontext.h>
#include <malloc.h>
#include <time.h>
#include <math.h>

#include "./ce-threads.h"

typedef struct
{
    ucontext_t context;
    int id;
    int active;
    int scheduler;
    clock_t time;
    double pause;
} cethread;

static cethread cethreadList[MAX_THREADS];
static ucontext_t mainContext;

static int currentCEthread = -1;
static int numCEthreads = 0;

void init_threads()
{
    for (int i = 0; i < MAX_THREADS; ++i)
    {
        cethreadList[i].active = 0;
    }
    return;
}

void CEthread_yield()
{
    if (numCEthreads == 0)
    {
        return;
    }
    currentCEthread = (currentCEthread + 1) % numCEthreads; /* Change context to next thread */
    if (cethreadList[currentCEthread].pause != 0)
    {
        clock_t end;
        double now = 0;
        end = clock();
        now = (double)(end) / (double)(CLOCKS_PER_SEC);

        double previous = (double)(cethreadList[currentCEthread].time) / (double)(CLOCKS_PER_SEC);
        while (now - previous < cethreadList[currentCEthread].pause)
        {
            end = clock();
            now = (double)(end) / (double)(CLOCKS_PER_SEC);
            currentCEthread = (currentCEthread + 1) % numCEthreads;
            previous = (double)(cethreadList[currentCEthread].time) / (double)(CLOCKS_PER_SEC);
        }

        usleep(2000000);
    }
}

static void CEthread_start(void (*func)(void))
{
    cethreadList[currentCEthread].active = 1;
    func();
    cethreadList[currentCEthread].active = 0;

    CEthread_yield(); /* Return control to parent thread*/
}

static void CEthread_start_A(void (*func)(int, int), int arg1, int arg2)
{
    cethreadList[currentCEthread].active = 1;
    func(arg1, arg2);
    cethreadList[currentCEthread].active = 0;

    CEthread_yield(); /* Return control to parent thread*/
}

int CEthread_create(void (*func)(void), int argc, int arg1, int arg2)
{
    if (numCEthreads == MAX_THREADS)
    {
        return MAXTHREADS;
    }

    getcontext(&cethreadList[numCEthreads].context); /* add the new function to the end of the list */

    /* Set context in a new stack */
    cethreadList[numCEthreads].context.uc_link = 0;
    cethreadList[numCEthreads].context.uc_stack.ss_sp = malloc(THREAD_STACK);
    cethreadList[numCEthreads].context.uc_stack.ss_size = THREAD_STACK;
    cethreadList[numCEthreads].context.uc_stack.ss_flags = 0;
    cethreadList[numCEthreads].id = numCEthreads;
    cethreadList[numCEthreads].pause = 0;
    cethreadList[numCEthreads].time = 0;

    if (cethreadList[numCEthreads].context.uc_stack.ss_sp == 0)
    {
        // CE_DEBUG_OUT("============> ERROR: NO se pudo inicializar un stack nuevo.", 0);
        return MALLOCERROR;
    }

    if (argc == 2)
    {
        makecontext(&cethreadList[numCEthreads].context, (void (*)(void)) & CEthread_start_A, func, arg1, arg2);
    }
    else
    {
        makecontext(&cethreadList[numCEthreads].context, (void (*)(void)) & CEthread_start, 1, func);
    }

    ++numCEthreads;

    return NOERROR;
}

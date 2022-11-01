#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../cethread.h"

int g = 0;

void *myThreadFun(void *vargp)
{
    int *myid = (int *)vargp;

    static int s = 0;

    ++s;
    ++g;

    for (int i = 0; i < 5; i++)
    {
        printf("Thread ID: %d, Jason: %d\n", *myid, ++g);
    }
}

int main()
{
    int i;

    cethread_t t1, t2;
    cethread_create(&t1, NULL, myThreadFun, (void *)&t1);
    cethread_create(&t2, NULL, myThreadFun, (void *)&t2);
        cethread_join(t1, t2);


    cethread_exit(NULL);
    return 0;
}
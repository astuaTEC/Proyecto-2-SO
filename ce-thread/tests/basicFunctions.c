#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../cethread.h"

void *myThreadFun(void *vargp)
{
    sleep(1);
    printf("Printing CETHREAD from cethread \n");
    return NULL;
}

int main()
{
    cethread_t thread_id;
    printf("Before thread\n");
    cethread_create(&thread_id, NULL, myThreadFun, NULL);
    cethread_join(thread_id, NULL);
    printf("After Thread\n");
    exit(0);
}
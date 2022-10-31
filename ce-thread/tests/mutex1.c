
#include <stdio.h>
#include <stdlib.h>

#include "../cethread.h"

void *functionC();
cemutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int  counter = 0;

main()
{
   int rc1, rc2;
   cethread_t thread1, thread2;

   /* Create independent threads each of which will execute functionC */

   if( (rc1=cethread_create( &thread1, NULL, &functionC, NULL)) )
   {
      printf("Thread creation failed: %d\n", rc1);
   }

   if( (rc2=cethread_create( &thread2, NULL, &functionC, NULL)) )
   {
      printf("Thread creation failed: %d\n", rc2);
   }

   /* Wait till threads are complete before main continues. Unless we  */
   /* wait we run the risk of executing an exit which will terminate   */
   /* the process and all threads before the threads have completed.   */

   cethread_join( thread1, NULL);
   cethread_join( thread2, NULL); 

   exit(0);
}

void *functionC()
{
   cemutex_lock( &mutex1 );
   counter++;
   printf("Counter value: %d\n",counter);
   cemutex_unlock( &mutex1 );
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "time.h" // Linux
#include <pthread.h>
#include <semaphore.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET "\x1B[0m"

typedef struct
{
    char type[20];
    int velocity;
    int pos; // por donde va dentro del canal
    int id;
    int direction; // 0 izquierda - 1 derecha
} ship ;


void *routine(void* data);

int channel[5] = {0, 0, 0, 0, 0}; //canal

sem_t sem;

int shipCount;

pthread_barrier_t barrier; 

void printchannel(int *channel, int length){
    int loop;
    for(loop = 0; loop < length; loop++)
      printf("%d ", channel[loop]);
      
   printf("\n");
}

int main(){

    pthread_t th_der[2], th_izq[2];
    ship* ships = (ship*) calloc(4, sizeof(ship));
    shipCount = 0;
    //pthread_barrier_init(&barrier, NULL, 4);

    if ( sem_init(&sem, 0, 1) != 0 )
    {
        // Error: initialization failed
        perror("Error: initialization failed");
    }

    for (int i = 0; i < 2; i++)
    {   
        shipCount++;
        ships[i].direction = 0;
        ships[i].id = shipCount;
        strcpy(ships[i].type, "Normal");
        ships[i].pos = -1;
        ships[i].velocity = 5;
        if(pthread_create(&th_izq[i], NULL, routine, &ships[i]) != 0){
            perror("Error creating the threads");
        };
    }

    int index = 0;
    for (int i = 0; i < 2; i++)
    {   
        index = i + shipCount;
        shipCount++;
        ships[index].direction = 1;
        ships[index].id = shipCount;
        strcpy(ships[index].type, "Pesquera");
        ships[index].pos = -1;
        ships[index].velocity = 6;
        if(pthread_create(&th_der[i], NULL, routine, &ships[index]) != 0){
            perror("Error creating the threads");
        };
    }

    for (int i = 0; i < 2; i++)
    {   
        if(pthread_join(th_izq[i], NULL) != 0){
            perror("Error joining the threads");
        };

        if(pthread_join(th_der[i], NULL) != 0){
            perror("Error joining the threads");
        };
    }

    sem_close(&sem);
    free(ships);

    return 0;
}

void *routine(void *data){

    //pthread_barrier_wait(&barrier);
    ship* s = (ship*) data;

    printf("ID... %d, Type: %s\n", s->id, s->type);

    int i;
    int length = sizeof channel / sizeof *channel;
    int sleepTime = length / s->velocity;
    for (i = 0; i < length; i++)
    {   

        sem_wait(&sem);
        if(channel[s->pos+1] == 0){ // esta disponible
            channel[s->pos+1] = s->id;
            s->pos++;
            if(s->pos >= 1)
                channel[s->pos - 1] = 0;
            printf("---------------\n");
            printchannel(channel, length);
            printf(KBLU "My id is %d\n" RESET, s->id);
            printf("---------------\n");
            sem_post(&sem);
            sleep(sleepTime);
        } else{
            i--;
            sem_post(&sem); 
        }
      
    }
    channel[length - 1] = 0;
    printf(KGRN "Ship id %d has finalized \n" RESET, s->id);
}
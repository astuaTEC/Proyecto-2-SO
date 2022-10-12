#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "time.h" // Linux
#include <pthread.h>
#include <semaphore.h>

typedef struct
{
    char type[20];
    int velocity;
    int pos;
    int id;
    int direction; // 0 izquierda - 1 derecha
} ship ;

typedef struct
{
	int shipNumber;
} shared_data_t;

void printArray(int *array, int length){
    int loop;
    for(loop = 0; loop < length; loop++)
      printf("%d ", array[loop]);
      
   printf("\n");
}

void *routine(void* data);

int array[5] = {0, 0, 0, 0, 0};

sem_t sem;

int shipCount;

pthread_barrier_t barrier;

int main(){

    pthread_t th_der[2], th_izq[2];
    ship* ships = (ship*) calloc(4, sizeof(ship));
    shipCount = 0;
    pthread_barrier_init(&barrier, NULL, 4);

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
        ships[i].pos = 0;
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
        ships[index].pos = 0;
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
    }

    for (int i = 0; i < 2; i++)
    {   
        if(pthread_join(th_der[i], NULL) != 0){
            perror("Error joining the threads");
        };
    }

    sem_close(&sem);
    free(ships);

    return 0;
}

void *routine(void *data){

    pthread_barrier_wait(&barrier);
    ship* s = (ship*) data;

    printf("ID... %d, Type: %s\n", s->id, s->type);

    int i, aux;
    int length = sizeof array / sizeof *array;
    int sleepTime = length / s->velocity;
    for (i = 0; i < length; i++)
    {   

        sem_wait(&sem);
        if(array[s->pos] == 0){
            array[s->pos] = s->id;
            if(i >= 1)
                array[s->pos - 1] = 0;
            printArray(array, length);
            s->pos++;
            sem_post(&sem);
            sleep(sleepTime);
        } else{
            i--;
            sem_post(&sem); 
        }
      
    }
    array[length - 1] = 0;
    printf("Ship id %d has finalized \n", s->id);
}
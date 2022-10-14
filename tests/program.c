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
void moverHaciaIzquierda(ship *s);
void moverHaciaDerecha(ship *s);

int channel[5] = {0, 0, 0, 0, 0}; //canal

sem_t sem, sem_lado;

int shipCount, defaultVel = 5;

int contIzq = 0, contDer = 0;

pthread_t th_der[2], th_izq[2];

void printArray(int *channel, int length){
    int loop;
    for(loop = 0; loop < length; loop++)
      printf("%d ", channel[loop]);
      
   printf("\n");
}

void createShips(char *path);

int main(int argc, char *argv[]){

    shipCount = 0;
    if ( sem_init(&sem, 0, 1) != 0 || sem_init(&sem_lado, 0, 2) != 0)
    {
        // Error: initialization failed
        perror("Error: initialization failed");
    }

    createShips("barcos.txt");

    sem_close(&sem);
    sem_close(&sem_lado);

    return 0;
}

void *routine(void *data){

    //pthread_barrier_wait(&barrier);
    ship* s = (ship*) data;

    printf("ID... %d, Type: %s, Vel: %d, Dir: %d\n", s->id, s->type, s->velocity, s->direction);

    if(s->direction == 0){
        contIzq++;
        moverHaciaDerecha(s);
    } else{
        contDer++;
        moverHaciaIzquierda(s);
    }
}

void createShips(char *path){
    FILE *fp = fopen(path, "r");

    if (fp == NULL)
    {
        printf("Error: could not open file %s", path);
    }

    // reading line by line, max 48 bytes
    const unsigned MAX_LENGTH = 48;
    char buffer[MAX_LENGTH];
    char delim[] = " ";
    int contIzq = 0, contDer = 0;

    char *ptr;
    ship *s;
    while (fgets(buffer, MAX_LENGTH, fp)){
        ptr = strtok(buffer, delim);

        /*****************************/
        s = (ship*) malloc(sizeof(ship));
        shipCount++;
        strcpy(s->type, ptr);
        s->id = shipCount;
        if(strcmp(ptr, "Normal") == 0) 
            s->velocity = defaultVel;
        else if (strcmp(ptr, "Pesquera") == 0) 
            s->velocity = defaultVel + 1;
        else 
            s->velocity = defaultVel + 2; // Patrulla

        /*******************************/
        //printf(KMAG "Ship type %s\n" RESET, ptr);

        ptr = strtok(NULL, delim);
        if(strcmp(ptr, "izq\n") == 0){
            s->pos = -1;
            s->direction = 0;
            if(pthread_create(&th_izq[contIzq], NULL, routine, &s[0]) != 0){
                perror("Error creating the threads");
            };
            contIzq++;
        }
        else {
            s->pos = 5; // lenght
            s->direction = 1;
            if(pthread_create(&th_der[contDer], NULL, routine, &s[0]) != 0){
                perror("Error creating the threads");
            };
            contDer++;
        }
        //printf(KCYN "Ship side %s\n" RESET, ptr);
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
    
    // close the file
    fclose(fp);
}

void moverHaciaDerecha(ship *s){
    int semValue;
    sem_getvalue(&sem_lado, &semValue);
    //printf("ID... %d, Type: %s, Vel: %d\n", s->id, s->type, s->velocity);
    if(contIzq == semValue){
        printf("Hacia Der\n");
    }
    sem_wait(&sem_lado);
    contIzq--;
    int i;
    int length = sizeof channel / sizeof *channel;
    int sleepTime = (int)( (length / s->velocity)*1e6 );
    for (i = 0; i < length; i++)
    {   

        sem_wait(&sem);
        if(channel[s->pos+1] == 0){ // esta disponible
            channel[s->pos+1] = s->id;
            s->pos++;
            if(s->pos >= 1)
                channel[s->pos - 1] = 0;
            
            printf("---------------\n");
            printArray(channel, length);
            printf(KBLU "My id is %d\n" RESET, s->id);
            printf("---------------\n");
            
            sem_post(&sem); // el post se debe hacer antes de los prints
                            // pero por cuestiones de desarrollo, se necesita
                            // ahi mientras tanto, para poder ver el comportamiento
            usleep(sleepTime);
        } else{
            i--;
            sem_post(&sem); 
        }
        
    }
    channel[length - 1] = 0;
    sem_post(&sem_lado);
    printf(KGRN "Ship id %d has finalized \n" RESET, s->id);
}

void moverHaciaIzquierda(ship *s){
    int semValue;
    sem_getvalue(&sem_lado, &semValue);
    //printf("ID... %d, Type: %s, Vel: %d\n", s->id, s->type, s->velocity);
    if(contDer == semValue){
        printf("Hacia Izq\n");
    }
    sem_wait(&sem_lado);
    contDer--;
    int i;
    int length = sizeof channel / sizeof *channel;
    int sleepTime = (int)( (length / s->velocity)*1e6 );
    for (i = length - 1; i >= 0; i--)
    {   

        sem_wait(&sem);
        if(channel[s->pos-1] == 0){ // esta disponible
            channel[s->pos-1] = s->id;
            s->pos--;

            channel[s->pos + 1] = 0;
            
            printf("---------------\n");
            printArray(channel, length);
            printf(KBLU "My id is %d\n" RESET, s->id);
            printf("---------------\n");
            
            sem_post(&sem); // el post se debe hacer antes de los prints
                            // pero por cuestiones de desarrollo, se necesita
                            // ahi mientras tanto, para poder ver el comportamiento
            usleep(sleepTime);
        } else{
            i++;
            sem_post(&sem); 
        }
        
    }
    channel[0] = 0;
    sem_post(&sem_lado);
    printf(KGRN "Ship id %d has finalized \n" RESET, s->id);
}
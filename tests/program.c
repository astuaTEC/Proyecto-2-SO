#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET "\x1B[0m"

/////////////// STRUCTS ////////////////////

typedef struct
{
    char type[20];
    int velocity;
    int pos; // por donde va dentro del canal
    int id;
    int direction; // 0 izquierda - 1 derecha
} ship ;

typedef struct {
    int *izqArray, *derArray;
} queueInfo;

////////////// ROUTINES ////////////////

void *routine(void* data);
void moverHaciaIzquierda(ship *s);
void moverHaciaDerecha(ship *s);
void createShips(char *path);
void initConfig(char *path);
void changeElement(int element, int *array, int length);
int64_t millis();
void controlLetrero();

/////////// VARIABLES //////////////////

int *channel; //canal

sem_t sem, sem_lado;

int channelSize = 0, readyShipSize = 0, semTime, W;

int shipCount, defaultVel = 5;

int contIzq = 0, contDer = 0, flagDir = 0;

pthread_t *th_der, *th_izq;

pthread_attr_t attr;

queueInfo * info;

char controlFlujo[20];

 time_t startTime, endTime;

///////////////////////////////////////////

void printArray(int *channel, int length){
    int loop;
    for(loop = 0; loop < length; loop++)
      printf("%d ", channel[loop]);
      
   printf("\n");
}

int main(int argc, char *argv[]){

    shipCount = 0;
    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_RR); //ROUND ROBIN
    initConfig("program.conf");

    if ( sem_init(&sem, 0, 1) != 0)
    {
        // Error: initialization failed
        perror("Error: initialization failed");
    }

    if (strcmp(controlFlujo, "Equidad") == 0){
        if (sem_init(&sem_lado, 0, W) != 0){
            // Error: initialization failed
            perror("Error: initialization failed");
        }
       
    } else if(strcmp(controlFlujo, "Tico") == 0 || strcmp(controlFlujo, "Letrero") == 0){
        W = 0;
        if (sem_init(&sem_lado, 0, readyShipSize) != 0){
            // Error: initialization failed
            perror("Error: initialization failed");
        }
    }

    info = (queueInfo *) malloc(sizeof(queueInfo));
    info->derArray = (int *) malloc(readyShipSize * sizeof(int));
    info->izqArray = (int *) malloc(readyShipSize * sizeof(int));
    for (int i = 0; i < readyShipSize; i++)
    {
        info->derArray[i] = 0 ;
        info->izqArray[i] = 0 ;
    }
    
    th_der = (pthread_t *) malloc(readyShipSize * sizeof(pthread_t));
    th_izq = (pthread_t *) malloc(readyShipSize * sizeof(pthread_t));

    channel = (int *) malloc(channelSize * sizeof(int));
    memset(channel, 0, channelSize * sizeof(int) ); // valores en cero

    createShips("barcos.txt");

    if(strcmp(controlFlujo, "Letrero") == 0) {
        controlLetrero();
    }

    // join the threads
    for (int i = 0; i < readyShipSize; i++)
    {   
        if(pthread_join(th_izq[i], NULL) != 0){
            perror("Error joining the threads");
        };

        if(pthread_join(th_der[i], NULL) != 0){
            perror("Error joining the threads");
        };
    }

    sem_close(&sem);
    sem_close(&sem_lado);
    free(channel);
    free(th_der);
    free(th_izq);
    free(info);

    return 0;
}

void *routine(void *data){
    ship* s = (ship*) data;

    printf("ID... %d, Type: %s, Vel: %d, Dir: %d\n", s->id, s->type, s->velocity, s->direction);

    if(s->direction == 0){
        info->izqArray[contIzq] = s->id;
        contIzq++;
        printf(KMAG "COLA IZQUIERDA...\n" RESET);
        printArray(info->izqArray, readyShipSize);
        printf(KMAG ".................\n" RESET);
        sleep(1);
        moverHaciaDerecha(s);
    } else{
        info->derArray[contDer] = s->id;
        contDer++;
        printf(KYEL "COLA DERECHA...\n" RESET);
        printArray(info->derArray, readyShipSize);
        printf(KYEL ".................\n" RESET);
        sleep(1);
        moverHaciaIzquierda(s);
    }
}

void initConfig(char *path){
    FILE *fp = fopen(path, "r");

    if (fp == NULL)
    {
        printf("Error: could not open file %s", path);
    }

    // reading line by line, max 48 bytes
    const unsigned MAX_LENGTH = 48;
    char buffer[MAX_LENGTH];
    char delim[] = "=";

    char *ptr;
    ship *s;
    while (fgets(buffer, MAX_LENGTH, fp)){
        ptr = strtok(buffer, delim);

        if( strcmp(ptr, "ControlFlujo") == 0){
            ptr = strtok(NULL, delim);
            strncpy(controlFlujo, ptr, strlen(ptr) - 1); // quitar el salto de linea
        } else if(strcmp(ptr, "LargoCanal") == 0){
            ptr = strtok(NULL, delim);
            channelSize = atoi(ptr);
        } else if(strcmp(ptr, "CantidadBarcosColaListos") == 0){
            ptr = strtok(NULL, delim);
            readyShipSize = atoi(ptr);
        } else if(strcmp(ptr, "VelocidadBarco") == 0){
            ptr = strtok(NULL, delim);
            defaultVel = atoi(ptr);
        } else if(strcmp(ptr, "TiempoLetrero") == 0){
            ptr = strtok(NULL, delim);
            semTime = atoi(ptr);
        } else if(strcmp(ptr, "W") == 0){
            ptr = strtok(NULL, delim);
            W = atoi(ptr);
        } 
    }

    // close the file
    fclose(fp);
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

        ptr = strtok(NULL, delim);
        if(strcmp(ptr, "izq\n") == 0){
            s->pos = -1;
            s->direction = 0;
            if(pthread_create(&th_izq[contIzq], &attr, routine, &s[0]) != 0){
                perror("Error creating the threads");
            };
            contIzq++;
        }
        else {
            s->pos = channelSize; // length
            s->direction = 1;
            if(pthread_create(&th_der[contDer], &attr, routine, &s[0]) != 0){
                perror("Error creating the threads");
            };
            contDer++;
        }
    }
    
    // close the file
    fclose(fp);
}

void moverHaciaDerecha(ship *s){
    sem_wait(&sem_lado);
    if(flagDir == 0 || flagDir == 1){ //verificar para que no hayan colisiones
        flagDir = 1;

        int i;
        int sleepTime = (int)( (channelSize / s->velocity)*1e6 );
        for (i = 0; i < channelSize; i++)
        {   
            sem_wait(&sem);
            if(channel[s->pos+1] == 0){ // esta disponible

                if(s->pos+1 == 0){
                    changeElement(s->id, info->izqArray, readyShipSize);
                    printf(KMAG "COLA IZQUIERDA...\n" RESET);
                    printArray(info->izqArray, readyShipSize);
                    printf(KMAG ".................\n" RESET);
                }
              
                channel[s->pos+1] = s->id;
                s->pos++;
                if(s->pos >= 1)
                    channel[s->pos - 1] = 0;
                
                printf("---------------\n");
                printf("My pos %d\n", s->pos);
                printArray(channel, channelSize);
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
        contIzq--;
        channel[channelSize - 1] = 0;
       
        printf("ContIzq %d\n", contIzq);
        printf(KGRN "Ship id %d has finalized \n" RESET, s->id);
        if(contIzq == 0 || (contIzq == readyShipSize - W) || (contIzq == 1 && W == 1)){
            if( strcmp(controlFlujo, "Letrero") != 0) flagDir = 2;
            printf("KKKKKKKK\n");
            int maxCount = 0;
            if(strcmp(controlFlujo, "Equidad") == 0) maxCount = W;
            else if (strcmp(controlFlujo, "Tico") == 0) maxCount = readyShipSize;
            for (int i = 0; i < maxCount; i++)
            {
                sem_post(&sem_lado);
            }
        }
        free(s);
    } else {
        sem_post(&sem_lado);
        moverHaciaDerecha(s);
    }
}

void moverHaciaIzquierda(ship *s){
    sem_wait(&sem_lado);
    if (flagDir == 0 || flagDir == 2){ //verificar para que no hayan colisiones
        flagDir = 2;

        int i;
        int sleepTime = (int)( (channelSize / s->velocity)*1e6 );
        for (i = channelSize - 1; i >= 0; i--)
        {   
            sem_wait(&sem);
            if(channel[s->pos-1] == 0){ // esta disponible

                if(s->pos - 1 == channelSize - 1){
                    changeElement(s->id, info->derArray, readyShipSize);
                    printf(KYEL "COLA DERECHA...\n" RESET);
                    printArray(info->derArray, readyShipSize);
                    printf(KYEL ".................\n" RESET);
                }
                
                channel[s->pos-1] = s->id;
                s->pos--;

                channel[s->pos + 1] = 0;
                
                printf("---------------\n");
                printf("My pos %d\n", s->pos);
                printArray(channel, channelSize);
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
        contDer--;
        channel[0] = 0;

        printf("ContDer %d\n", contDer);
        printf(KGRN "Ship id %d has finalized \n" RESET, s->id);
        if(contDer == 0 || contDer == (readyShipSize - W) || (contDer == 1 && W == 1)){
            if( strcmp(controlFlujo, "Letrero") != 0) flagDir = 1;
            printf("OOOOOOOOOOO\n");
            int maxCount = 0;
            if(strcmp(controlFlujo, "Equidad") == 0) maxCount = W;
            else if (strcmp(controlFlujo, "Tico") == 0) maxCount = readyShipSize;
            for (int i = 0; i < maxCount; i++)
            {
                sem_post(&sem_lado);
            }
        }
        free(s);
    } else {
        sem_post(&sem_lado);
        moverHaciaIzquierda(s);
    }
}

void changeElement(int element, int *array, int length){
    for (int i = 0; i < length; i++)
    {
        if(array[i] == element){
            array[i] = 0;
        }
        if( i > 0 && array[i] != 0&& array[i-1] == 0){
            array[i-1] = array[i];
            array[i] = 0;
            continue; 
        }
    }
    
}

int64_t millis()
{
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    return ((int64_t) now.tv_sec) * 1000 + ((int64_t) now.tv_nsec) / 1000000;
}

void controlLetrero(){
    usleep(semTime*1000);
    printf("BRRRRRRR\n");
    if(flagDir == 1 && contIzq == 0){
        flagDir = 2;
        for (int i = 0; i < readyShipSize; i++){
            sem_post(&sem_lado);
        }
    } else if(flagDir == 2 && contDer == 0) {
        flagDir = 1;
        for (int i = 0; i < readyShipSize; i++){
            sem_post(&sem_lado);
        }
    } 
    if(contDer == 0 && contIzq == 0){
        printf("YAAAAAAAAA\n");
        return;
    }

    return controlLetrero();
}
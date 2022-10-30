#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include "../arduino-serial/arduino-serial-lib.h"

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
void prepareArduinoList();
void keyManager();

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

int *arduinoArray;

int rAux = 0;

int fd; // Arduino

///////////////////////////////////////////

void printArray(int *channel, int length){
    int loop;
    for(loop = 0; loop < length; loop++)
      printf("%d ", channel[loop]);
      
   printf("\n");
}

void setScheduler(char *sch){
    pthread_attr_init(&attr);
    if(strcmp(sch, "RR") == 0){
        printf("ROUND ROBIN\n");
        pthread_attr_setschedpolicy(&attr, SCHED_RR); //ROUND ROBIN
    } else if(strcmp(sch, "FCFS") == 0){
        printf("FCFS\n");
        pthread_attr_setschedpolicy(&attr, SCHED_FIFO); //FIFO
    } else {
        printf("OTHER\n");
        pthread_attr_setschedpolicy(&attr, SCHED_OTHER); //OTHER
    }
}

int main(int argc, char *argv[]){

    if(argc < 2){
        perror("Missing arguments");
        return 1;
    }

    setScheduler(argv[1]);

    ////////////// SERIAL ///////////////

    char *serialport = "/dev/ttyACM0";
    int baudrate = 9600;

    fd = serialport_init(serialport, baudrate);

    if (fd < 0){
        printf(KRED "serial port initialization failed\n" RESET);
        return fd;
    }

    printf("successfully opened serialport %s @ %d bps\n", serialport, baudrate);
    serialport_flush(fd);

    ////////////////////////////////////////

    shipCount = 0;

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
       
    } else if(strcmp(controlFlujo, "Letrero") == 0){
        W = 0;
        if (sem_init(&sem_lado, 0, readyShipSize) != 0){
            // Error: initialization failed
            perror("Error: initialization failed");
        }
    } else if(strcmp(controlFlujo, "Tico") == 0){
        W = 0;
        srand(time(NULL));
        int r = rand() % (readyShipSize) + 1; // entre 1 y readyShipSize
        rAux = r;
        printf("RRR %d \n", r);
        if (sem_init(&sem_lado, 0, r) != 0){
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

    arduinoArray = (int *) malloc((2*readyShipSize + channelSize+ 1) * sizeof(int));
    memset(arduinoArray, 0, (2*readyShipSize + channelSize + 1) * sizeof(int)); // valores en cero

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

    //keyManager();
    serialport_close(fd);
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
        prepareArduinoList();
        sleep(1);
        moverHaciaDerecha(s);
    } else{
        info->derArray[contDer] = s->id;
        contDer++;
        prepareArduinoList();
        sleep(1);
        moverHaciaIzquierda(s);
    }

    return s;
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
        int sleepTime = (int)( (channelSize / s->velocity)*2e6 );
        for (i = 0; i < channelSize; i++)
        {   
            sem_wait(&sem);
            if(channel[s->pos+1] == 0){ // esta disponible

                if(s->pos+1 == 0){
                    changeElement(s->id, info->izqArray, readyShipSize);
                }
              
                channel[s->pos+1] = s->id;
                s->pos++;
                if(s->pos >= 1)
                    channel[s->pos - 1] = 0;
                
                /*printf("---------------\n");
                printf(KMAG "My pos %d\n", s->pos);
                printArray(channel, channelSize);*/
                prepareArduinoList();
                /*printf("My id is %d\n" RESET, s->id);
                printf("---------------\n");*/
                
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

        if (strcmp(controlFlujo, "Tico") == 0) rAux--;

        prepareArduinoList();
        printf("ContIzq %d\n", contIzq);

        printf(KGRN "Ship id %d has finalized \n" RESET, s->id);

        if(contIzq == 0 || (contIzq == readyShipSize - W) || (contIzq <= (readyShipSize - W)&& W == 1)
         || (strcmp(controlFlujo, "Tico") == 0 && rAux == 0) ){
            if( strcmp(controlFlujo, "Letrero") != 0) flagDir = 2;
            printf("KKKKKKKK\n");
            int maxCount = 0;
            if(strcmp(controlFlujo, "Equidad") == 0) {
                if(contDer == 1) maxCount = 1;
                else maxCount = W;
            }
            else if (strcmp(controlFlujo, "Tico") == 0){
                srand(time(NULL));
                int r = rand() % (readyShipSize) + 1; // entre 1 y readyShipSize
                while(r > contDer && contDer != 0){
                    r = rand() % (readyShipSize) + 1; // entre 1 y readyShipSize
                }
                printf("RRR %d \n", r);
                rAux = r;
                maxCount = r;
            }
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
        int sleepTime = (int)( (channelSize / s->velocity)*2e6 );
        for (i = channelSize - 1; i >= 0; i--)
        {   
            sem_wait(&sem);
            if(channel[s->pos-1] == 0){ // esta disponible

                if(s->pos - 1 == channelSize - 1){
                    changeElement(s->id, info->derArray, readyShipSize);
                    /*printf(KYEL "COLA DERECHA...\n" RESET);
                    printArray(info->derArray, readyShipSize);
                    printf(KYEL ".................\n" RESET);*/
                }
                
                channel[s->pos-1] = s->id;
                s->pos--;

                channel[s->pos + 1] = 0;
                
                /*printf("---------------\n");
                printf(KBLU "My pos %d\n", s->pos);
                printArray(channel, channelSize);*/
                prepareArduinoList();
                /*printf("My id is %d\n" RESET, s->id);
                printf("---------------\n");*/
                
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

        if (strcmp(controlFlujo, "Tico") == 0) rAux--;

        prepareArduinoList();
        printf("ContDer %d\n", contDer);

        printf(KGRN "Ship id %d has finalized \n" RESET, s->id);

        if(contDer == 0 || contDer == (readyShipSize - W) || (contDer <= (readyShipSize - W) && W == 1) 
        || (strcmp(controlFlujo, "Tico") == 0 && rAux == 0) ){
            if( strcmp(controlFlujo, "Letrero") != 0) flagDir = 1;
            printf("OOOOOOOOOOO\n");
            int maxCount = 0;
            if(strcmp(controlFlujo, "Equidad") == 0) {
                if(contIzq == 1) maxCount = 1;
                else maxCount = W;
            }
            else if (strcmp(controlFlujo, "Tico") == 0){
                srand(time(NULL));
                int r = rand() % (readyShipSize) + 1; // entre 1 y readyShipSize
                while(r > contIzq && contIzq != 0){
                    r = rand() % (readyShipSize) + 1; // entre 1 y readyShipSize
                }
                printf("RRR %d \n", r);
                rAux = r;
                maxCount = r;
            }
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
        if( i > 0 && array[i] != 0 && array[i-1] == 0){
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

void prepareArduinoList(){;
    int rc;
    char buffer[512];
    bzero(buffer, 512); //clear the buffer

    int i;
    for (i = 0; i < readyShipSize; i++)
    {
        arduinoArray[i] = info->izqArray[i];
        arduinoArray[i + readyShipSize + channelSize] = info->derArray[i];
    }

    for (i = 0; i < channelSize; i++)
    {
        arduinoArray[i + readyShipSize] = channel[i];
    }

    arduinoArray[2*readyShipSize + channelSize] = flagDir;
    int index = 0;
    for (i = 0; i < (2*readyShipSize + channelSize + 1); i++)
    {
        index += snprintf(&buffer[index], 128-index, "%d", arduinoArray[i]);
    }
    
    rc = serialport_write(fd, buffer); 

    if(rc < 0){
        printf( KRED "Writing to serial port failed rc: %d\n" RESET, rc);
    }

    /*printf("///////////////\n");
    printArray(arduinoArray, 2*readyShipSize + channelSize);
    printf("///////////////\n ");*/
}

/*
void keyManager(){
    char *ch;
    printf("Presione una tecla: ");
    scanf("%c", ch);
    if(strcmp(ch, "w") == 0){
        if(contIzq < readyShipSize){
            ship *s = (ship*) malloc(sizeof(ship));
            s->velocity = defaultVel;
            strcpy(s->type, "Normal");
            
            s->pos = -1;
            s->direction = 0;
            if(pthread_create(&th_izq[contIzq], &attr, routine, &s[0]) != 0){
                perror("Error creating the threads");
            };

            if(pthread_join(th_izq[contIzq], NULL) != 0){
            perror("Error joining the threads");
            };
        }

    } else if (strcmp(ch, "s") == 0){

    } else if (strcmp(ch, "x") == 0){
        
    } else if(strcmp(ch, "e") == 0){

    } else if (strcmp(ch, "d") == 0){

    } else if (strcmp(ch, "c")){

    } else if (strcmp(ch, "q")){
        return;
    }
    return keyManager();
}*/
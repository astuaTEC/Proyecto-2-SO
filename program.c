#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> // Windows
//#include <time.h> // Linux

typedef struct
{
    char type[20];
    int velocity;
    int pos;

} ship ;

void printArray(int *array, int length){
    int loop;
    for(loop = 0; loop < length; loop++)
      printf("%d ", array[loop]);
      
   printf("\n");
}

int main(){

    int array[5] = {0, 0, 0, 0, 0};

    ship* s = (ship*) malloc(sizeof(ship));

    s->pos = 0;
    s->velocity = 5;
    strcpy(s->type, "normal");

    int i, aux;
    int length = sizeof array / sizeof *array;
    int sleepTime = length / s->velocity;
    for (i = 0; i < length; i++)
    {   
        s->pos = i;
        aux = s->pos;
        array[s->pos] = 1;
        printArray(array, length);
        Sleep(sleepTime*1000);

        array[aux] = 0;
    }
    
    free(s);

    return 0;
}
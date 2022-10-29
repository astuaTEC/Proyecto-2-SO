#include "./scheduler.h"

void schedpolicy(int schedulerType)
{
    switch (schedulerType)
    {
    case 0:
        printf("============> ROUND ROBIN: ");
        break;
    case 1:
        printf("============> PRIORITY: ");
        break;
    case 2:
        printf("============> SJF: ");
        break;
    case 3:
        printf("============> FCFS: ");
        break;
    case 4:
        printf("============> REAL TIME: ");
        break;
    default:
        break;
    }
}
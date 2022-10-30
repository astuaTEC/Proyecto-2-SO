#include "./scheduler.h"

void schedpolicy(int schedulerType, int d, int qt)
{
    switch (schedulerType)
    {
    case 0:
        printf("============> ROUND ROBIN: ");
        {
            int n, i, tempn, count, terminaltime = 0, initialtime, qt, flag = 0, *bt, *wt, *tat, *tempbt;
            float avgwt = 0, avgtat = 0;
            printf("\n Enter the number of processes : ");
            scanf("%d", &n);
            tempn = n;

            tempbt = (int *)malloc(n * sizeof(int));
            bt = (int *)malloc(n * sizeof(int));
            wt = (int *)malloc(n * sizeof(int));
            tat = (int *)malloc(n * sizeof(int));

            for (i = 0; i < n; i++)
            {
                printf(" Burst time of P%d : ", i);
                scanf("%d", &bt[i]);
                tempbt[i] = bt[i];
                terminaltime += bt[i];
            }

            wt[0] = 0;

            for (terminaltime = 0, count = 0; tempn != 0;)
            {
                initialtime = terminaltime;
                if (tempbt[count] <= qt && tempbt[count] > 0)
                {
                    terminaltime += tempbt[count];
                    tempbt[count] = 0;
                    wt[count] = terminaltime - bt[count];
                    tat[count] = wt[count] + bt[count];
                    flag = 1;
                }
                else if (tempbt[count] > qt)
                {
                    tempbt[count] -= qt;
                    terminaltime += qt;
                }
                if (tempbt[count] == 0 && flag == 1)
                {
                    tempn--;
                    flag = 0;
                }
                if (initialtime != terminaltime)
                {
                    printf(" %d\t|| P%d ||\t%d\n", initialtime, count, terminaltime);
                }
                if (count == n - 1)
                    count = 0;
                else
                    ++count;
            }

            for (i = 0; i < n; i++)
            {
                printf(" P%d \t\t %d \t\t %d \t\t %d \n", i, bt[i], wt[i], tat[i]);
            }

            for (i = 0; i < n; i++)
            {
                avgwt += wt[i];
                avgtat += tat[i];
            }
            avgwt = avgwt / n;
            avgtat = avgtat / n;

            printf("\n Average Waiting Time = %f \n Average Turnaround Time = %f \n", avgwt, avgtat);

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
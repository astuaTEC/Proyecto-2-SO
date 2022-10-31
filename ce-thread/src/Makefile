CC = gcc
CFLAGS = -g -w -Wall -Werror -std=c99

all:: roundRobin priority sjf fcfs
 
roundRobin:
	$(CC) $(CFLAGS) -o roundRobin roundRobin.c -L../ 

priority:
	$(CC) $(CFLAGS) -o priority priority.c -L../ 

sjf:
	$(CC) $(CFLAGS) -o sjf sjf.c -L../ 

fcfs:
	$(CC) $(CFLAGS) -o fcfs fcfs.c -L../ 

clean:
	rm -rf roundRobin priority sjf fcfs *.o ./record/

CC = gcc
CFLAGS = -g -c
AR = ar -rc
RANLIB = ranlib

SCHED = PSJF

all: cethread.a

cethread.a: cethread.o
	$(AR) libcethread.a cethread.o
	$(RANLIB) libcethread.a

cethread.o: cethread.h

ifeq ($(SCHED), PSJF)
	$(CC) -pthread $(CFLAGS) cethread.c
else ifeq ($(SCHED), MLFQ)
	$(CC) -pthread $(CFLAGS) -DMLFQ cethread.c
else
	echo "no such scheduling algorithm"
endif

clean:
	rm -rf testfile *.o *.a

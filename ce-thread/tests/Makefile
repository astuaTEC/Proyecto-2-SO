CC = gcc
CFLAGS = -g -w -Wall -Werror -std=c99

all:: basicFunctions multipleThreads creations mutex1

basicFunctions:
	$(CC) $(CFLAGS) -o basicFunctions basicFunctions.c -L../ -lcethread

multipleThreads:
	$(CC) $(CFLAGS) -o multipleThreads multipleThreads.c -L../ -lcethread

creations:
	$(CC) $(CFLAGS) -o creations creations.c -L../ -lcethread

mutex1:
	$(CC) $(CFLAGS) -o mutex1 mutex1.c -L../ -lcethread

clean:
	rm -rf testcase basicFunctions multipleThreads creations mutex1 *.o ./record/

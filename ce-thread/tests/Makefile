CC = gcc
CFLAGS = -g -w -Wall -Werror -std=c99

all:: basicFunctions


basicFunctions:
	$(CC) $(CFLAGS) -o basicFunctions basicFunctions.c -L../ -lcethread

clean:
	rm -rf testcase basicFunctions *.o ./record/

# (2 pts) Build a makefile that creates an executable called queuetest

CC = gcc
CFLAGS = -Wall -Wextra -g

all: queuetest

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c

queuetest.o: queuetest.c queue.h
	$(CC) $(CFLAGS) -c queuetest.c

queuetest: queue.o queuetest.o
	$(CC) $(CFLAGS) -o queuetest queue.o queuetest.o

clean:
	rm -f *.o queuetest

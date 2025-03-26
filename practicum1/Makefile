# Makefile for message_store project

CC = gcc
CFLAGS = -Wall -Wextra -g

# Object files
OBJS = main.o message.o cache.o

# Target binary
TARGET = message_store

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c message.h cache.h
	$(CC) $(CFLAGS) -c main.c

message.o: message.c message.h
	$(CC) $(CFLAGS) -c message.c

cache.o: cache.c cache.h message.h
	$(CC) $(CFLAGS) -c cache.c

clean:
	rm -f *.o $(TARGET)

# Makefile

CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11

# Phony target to build everything
all: multi_process cipher

# -------------------------------
# Build the multi_process program
multi_process: multi_process.o queue.o
	$(CC) $(CFLAGS) multi_process.o queue.o -o multi_process

multi_process.o: multi_process.c queue.h
	$(CC) $(CFLAGS) -c multi_process.c -o multi_process.o

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c -o queue.o

# -------------------------------
# Build the cipher program
cipher: cipher.o polybius.o
	$(CC) $(CFLAGS) cipher.o polybius.o -o cipher

cipher.o: cipher.c
	$(CC) $(CFLAGS) -c cipher.c -o cipher.o

polybius.o: polybius.c polybius.h
	$(CC) $(CFLAGS) -c polybius.c -o polybius.o

# -------------------------------
# Clean up
clean:
	rm -f *.o multi_process cipher \
	      chunk_*.txt cipher_output_*.txt strings.txt

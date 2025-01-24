CC=gcc
CFLAGS=-Wall -g

# Default target
all: learnc arger

# Target for learnc that depends on object files for mpg2km and learnc
learnc: learnc.o mpg2km.o
	$(CC) $(CFLAGS) -o learnc learnc.o mpg2km.o

# Separate compilation of learnc.c
learnc.o: learnc.c mpg2km.h
	$(CC) $(CFLAGS) -c learnc.c

# Separate compilation of mpg2km.c
mpg2km.o: mpg2km.c mpg2km.h
	$(CC) $(CFLAGS) -c mpg2km.c

# Target for arger which may also have dependencies in the future
arger: arger.c
	$(CC) $(CFLAGS) -o arger arger.c

# Clean up build artifacts
clean:
	rm -f learnc arger *.o



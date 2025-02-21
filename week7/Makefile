# Makefile - single file for building Part1 and Part3 programs.

CC      = gcc
CFLAGS  = -Wall -pthread
EXEC1   = multithread
EXEC2   = thread_cancel
EXEC3   = thread_shared
EXEC4   = pqueuepmain

all: $(EXEC1) $(EXEC2) $(EXEC3) $(EXEC4)

#####################################
# Part 1 Programs
#####################################

$(EXEC1): multithread.c
	$(CC) $(CFLAGS) -o $@ multithread.c

$(EXEC2): thread_cancel.c
	$(CC) $(CFLAGS) -o $@ thread_cancel.c

$(EXEC3): thread_shared.c
	$(CC) $(CFLAGS) -o $@ thread_shared.c

#####################################
# Part 3 Program (Producer-Consumer)
#####################################

$(EXEC4): pqueuepmain.c pqueuep.c queuep.c pqueuep.h queuep.h
	$(CC) $(CFLAGS) -o $@ pqueuepmain.c pqueuep.c queuep.c

#####################################
# Cleanup
#####################################

clean:
	rm -f $(EXEC1) $(EXEC2) $(EXEC3) $(EXEC4)

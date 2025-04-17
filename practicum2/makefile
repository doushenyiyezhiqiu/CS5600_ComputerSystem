#
# makefile for multi-client RFS with permissions
#

CC     = gcc
CFLAGS = -Wall -Wextra -g

SERVER_SRCS = server.c
CLIENT_SRCS = client.c

SERVER_OBJS = $(SERVER_SRCS:.c=.o)
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)

all: rfserver rfs

rfserver: $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o rfserver $(SERVER_OBJS) -lpthread

rfs: $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o rfs $(CLIENT_OBJS)

%.o: %.c common.h server.h client.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f rfserver rfs *.o

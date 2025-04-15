#
# Simple makefile for the RFS client/server project
#

CC     = gcc
CFLAGS = -Wall -Wextra -g

# Server and Client source files
SERVER_SRCS = server.c
SERVER_HDRS = server.h
CLIENT_SRCS = client.c
CLIENT_HDRS = client.h
COMMON_HDRS = common.h

# Object files
SERVER_OBJS = $(SERVER_SRCS:.c=.o)
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)

all: rfserver rfs

rfserver: $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o rfserver $(SERVER_OBJS)

rfs: $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o rfs $(CLIENT_OBJS)

%.o: %.c $(SERVER_HDRS) $(CLIENT_HDRS) $(COMMON_HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f rfserver rfs *.o

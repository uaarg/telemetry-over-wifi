CC := gcc
CC_FLAGS := -Wall

all:		client server
	#
client:		client.c
	$(CC) $(CC_FLAGS) client.c -o client

server:		redServer.h streamServer.c
	$(CC) $(CC_FLAGS) streamServer.c -o server

clean:
	[ -f server ] && rm server;
	[ -f client ] && rm client;

ifeq ($(OS), Windows_NT) 
	PLATFORM  = WINDOWS
	CLIENT_EXEC = client.exe
	SERVER_EXEC = server.exe
	SYS_LIBS = -lws2_32
	RM = del
else
	PLATFORM = UNIX
	CLIENT_EXEC = client
	SERVER_EXEC = server
	RM = rm -f
endif
#Credit to Konrad Lindenbach <konrad.lindenbach@gmail.com>
#for the above cross-platform compilation clauses

all:	Makefile $(CLIENT_EXEC) $(SERVER_EXEC)
	#

LIBS := $(SYS_LIBS) -pthread
CC := gcc
CC_FLAGS := -Wall

$(CLIENT_EXEC):	      Makefile client.c ioLib connection
	$(CC) -D$(PLATFORM) $(LIBS) $(CC_FLAGS) ioLib.o client.c connections.o -o $(CLIENT_EXEC)

$(SERVER_EXEC):	      Makefile redServer.h streamServer.c ioLib connection
	$(CC) -D$(PLATFORM) $(LIBS) $(CC_FLAGS) ioLib.o streamServer.c connections.o -o $(SERVER_EXEC)

ioLib:	      ioLib.h ioLib.c dataTypes.h
	$(CC) $(CC_FLAGS) -c ioLib.c -o ioLib.o

connection:	    connections.h connections.c
	$(CC) $(CC_FLAGS) -D$(PLATFORM) -c connections.c -o connections.o

clean:
	$(RM) *.o
	test -f $(SERVER_EXEC) && $(RM) $(SERVER_EXEC);
	test -f $(CLIENT_EXEC) && $(RM) $(CLIENT_EXEC);

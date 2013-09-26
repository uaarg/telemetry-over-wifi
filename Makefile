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
	echo $(OS)
	#

LIBS := $(SYS_LIBS) -pthread
CC := gcc
CC_FLAGS := -Wall

$(CLIENT_EXEC):	      Makefile src/client.c ioLib connection
	$(CC) -D$(PLATFORM) $(LIBS) $(CC_FLAGS) exec/sigHandling.o exec/ioLib.o exec/connections.o src/client.c -o exec/$(CLIENT_EXEC)

$(SERVER_EXEC):	      Makefile src/streamServer.c ioLib connection
	$(CC) -D$(PLATFORM) $(LIBS) $(CC_FLAGS) exec/sigHandling.o exec/ioLib.o exec/connections.o  src/streamServer.c -o exec/$(SERVER_EXEC)

connection:	    include/connections.h src/connections.c sigHandling ioLib
	$(CC) $(CC_FLAGS) -D$(PLATFORM) -c src/connections.c -o exec/connections.o

ioLib:	      include/ioLib.h src/ioLib.c include/dataTypes.h
	$(CC) $(CC_FLAGS) -c src/ioLib.c -o exec/ioLib.o


sigHandling:  include/sigHandling.h src/sigHandling.c include/dataTypes.h ioLib
	$(CC) $(CC_FLAGS) -c src/sigHandling.c -o exec/sigHandling.o

clean:
	$(RM) exec/*.o
	test -f exec/$(SERVER_EXEC) && $(RM) exec/$(SERVER_EXEC);
	test -f exec/$(CLIENT_EXEC) && $(RM) exec/$(CLIENT_EXEC);

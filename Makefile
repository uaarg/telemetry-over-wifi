ifeq ($(OS), Windows_NT) 
	PLATFORM  = WINDOWS
	CLIENT_EXEC = client.exe
	SERVER_EXEC = server.exe
	SYS_LIBS = -lws2_32
	RM = del
	echo "ERROR WINDOWS NOT SUPPORTED"
else
	PLATFORM = UNIX
	CLIENT_EXEC = client
	SERVER_EXEC = server
	RM = rm -f
	RM_GEN = $(RM) {} \;
endif

#Credit to Konrad Lindenbach <konrad.lindenbach@gmail.com>
#for the above cross-platform compilation clauses

all:	Makefile $(CLIENT_EXEC) $(SERVER_EXEC)

LIBS := $(SYS_LIBS) -pthread
CC := gcc
CC_FLAGS := -Wall

$(CLIENT_EXEC):	Makefile src/client.c ioLib connection polling
	$(CC) -D$(PLATFORM) $(LIBS) $(CC_FLAGS) exec/sigHandling.o exec/ioLib.o exec/cserial.o exec/connections.o exec/polling.o src/client.c -o exec/$(CLIENT_EXEC)

$(SERVER_EXEC):	Makefile src/streamServer.c ioLib connection polling
	$(CC) -D$(PLATFORM) $(LIBS) $(CC_FLAGS) exec/sigHandling.o exec/ioLib.o exec/cserial.o exec/connections.o  exec/polling.o src/streamServer.c -o exec/$(SERVER_EXEC)

connection:	include/connections.h src/connections.c sigHandling ioLib cserial
	$(CC) $(CC_FLAGS) -D$(PLATFORM) -c src/connections.c -o exec/connections.o

ioLib:	include/ioLib.h src/ioLib.c include/dataTypes.h
	$(CC) $(CC_FLAGS) -c src/ioLib.c -o exec/ioLib.o

cserial:	include/cserial.h src/cserial.c include/dataTypes.h
	$(CC) $(CC_FLAGS) -c src/cserial.c -o exec/cserial.o

sigHandling:	include/sigHandling.h src/sigHandling.c include/dataTypes.h ioLib cserial
	$(CC) $(CC_FLAGS) -c src/sigHandling.c -o exec/sigHandling.o


polling:	include/polling.h include/constants.h src/polling.c
	$(CC) $(CC_FLAGS) -c src/polling.c -o exec/polling.o

clean:
	cd exec/
	find -type f -name "*.o" -exec $(RM_GEN)
	find -name "$(SERVER_EXEC)" -exec $(RM_GEN)
	find -name "$(CLIENT_EXEC)" -exec $(RM_GEN)

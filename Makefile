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

$(CLIENT_EXEC):	Makefile src/client.c exec/ioLib.o exec/connections.o exec/polling.o exec/slist.o
	$(CC) -D$(PLATFORM) $(LIBS) $(CC_FLAGS) exec/SList.o exec/sigHandling.o exec/ioLib.o exec/cserial.o exec/connections.o exec/polling.o src/client.c -o exec/$(CLIENT_EXEC)

$(SERVER_EXEC):	Makefile src/streamServer.c exec/ioLib.o exec/connections.o exec/polling.o exec/slist.o
	$(CC) -D$(PLATFORM) $(LIBS) $(CC_FLAGS) exec/SList.o exec/sigHandling.o exec/ioLib.o exec/cserial.o exec/connections.o  exec/polling.o src/streamServer.c -o exec/$(SERVER_EXEC)

exec/connections.o: include/connections.h src/connections.c exec/sigHandling.o exec/ioLib.o exec/cserial.o
	$(CC) $(CC_FLAGS) -D$(PLATFORM) -c src/connections.c -o exec/connections.o

exec/ioLib.o:    include/ioLib.h src/ioLib.c include/dataTypes.h
	$(CC) $(CC_FLAGS) -c src/ioLib.c -o exec/ioLib.o

exec/cserial.o:	include/cserial.h src/cserial.c include/dataTypes.h
	$(CC) $(CC_FLAGS) -c src/cserial.c -o exec/cserial.o

exec/sigHandling.o:	include/sigHandling.h src/sigHandling.c include/dataTypes.h exec/ioLib.o exec/cserial.o
	$(CC) $(CC_FLAGS) -c src/sigHandling.c -o exec/sigHandling.o


exec/polling.o:	include/polling.h include/constants.h src/polling.c
	$(CC) $(CC_FLAGS) -c src/polling.c -o exec/polling.o

exec/slist.o:		include/SList.h src/SList.c
	$(CC) $(CC_FLAGS) -c src/SList.c -o exec/SList.o

clean:
	cd exec/
	find -type f -name "*.o" -exec $(RM_GEN)
	find -name "$(SERVER_EXEC)" -exec $(RM_GEN)
	find -name "$(CLIENT_EXEC)" -exec $(RM_GEN)

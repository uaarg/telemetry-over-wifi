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

LIBS := $(SYS_LIBS)
CC := gcc
CC_FLAGS := -Wall

$(CLIENT_EXEC):	      Makefile client.c
	$(CC) -D$(PLATFORM) $(LIBS) $(CC_FLAGS) client.c -o $(CLIENT_EXEC)

$(SERVER_EXEC):	      Makefile redServer.h streamServer.c
	$(CC) -D$(PLATFORM) $(LIBS) $(CC_FLAGS) streamServer.c -o $(SERVER_EXEC)

clean:
	test -f $(SERVER_EXEC) && $(RM) $(SERVER_EXEC);
	test -f $(CLIENT_EXEC) && $(RM) $(CLIENT_EXEC);

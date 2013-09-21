#ifndef _CONNECTIONS_H
#define _CONNECTIONS_H
  #include <stdio.h>
  #include <errno.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <string.h>
  #include <termios.h>

  #include "platformHandler.h"
  #include "ioLib.h"

  #include "constants.h"
  #include "dataTypes.h"

  void initBiSocket(BiSocket *);

  //Get sockaddr, IPv4 or IPv6:
  void *get_in_addr(struct sockaddr *);
  
  //Takes args: HostName, PORT both words/character sequences
  int socketConnection(const word, const word);

  long long int sendData(fdPair *, struct timeval);

  long long int recvData(fdPair *, struct timeval);

  //Handler for outgoing data to be pushed to the connected end of
  //a client socket
  void *outMsgHandler(void *);
  
  //Handler for incoming data pushed into a client socket
  void *inMsgHandler(void *);

  //Thread-based function to manage sending and receiving of data
  //based off the states 'RECEIVING' and 'SENDING'
  void *msgTransit(void *);
#endif
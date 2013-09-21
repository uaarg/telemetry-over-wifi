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

  void *msgTransit(void *);
#endif

// Author: Emmanuel Odeke <odeke@ualberta.ca>

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
  #include "sigHandling.h"

  #include "constants.h"
  #include "dataTypes.h"

  //Get sockaddr, IPv4 or IPv6:
  void *get_in_addr(struct sockaddr *);
  
  //Takes args: HostName, PORT both words/character sequences
  int socketConnection(const word, const word);

  //Given a struct containing hostName and port as attributes
  //invoke and return the result from socketConnection(...)
  void *socketViaStruct(void *);

  //Returns the long long int byte count of sent data
  LLInt sendData(fdPair *, struct timeval);

  //Returns the long long int byte count of received data
  LLInt recvData(fdPair *, struct timeval);

  //Handler for outgoing data to be pushed to the connected end of
  //a client socket
  void *outMsgHandler(void *);
  
  //Handler for incoming data pushed into a client socket
  void *inMsgHandler(void *);

  //Thread-based function to manage sending and receiving of data
  //based off the states 'RECEIVING' and 'SENDING'
  //RETURNS the total byte count: 
  // PLEASE REMEMBER TO FREE THE MEMORY ALLOCATED TO RETURN THE BYTE COUNT
  void *msgTransit(void *);

  //Given a port, an a file pointer for the data to be 
  //received through the port, spawn a server
  int runServer(const word, FILE *);
#endif

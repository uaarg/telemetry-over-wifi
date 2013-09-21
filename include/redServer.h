/*
  Author: Emmanuel Odeke <odeke@ualberta.ca>
  Redundant links server
*/

#ifndef _SERVER_H
#define _SERVER_H
  #include <errno.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <signal.h> 
  #include <stdio.h>
  #include <string.h>

#ifdef UNIX
  #include <termios.h>
#endif

  #include "ioLib.h"
  #include "dataTypes.h"
  #include "connections.h"
  #include "platformHandler.h" 

  #define BACKLOG 20 //How many pending connections queue will hold
  #define BUF_SIZ 60 //Arbitrary size here
  void terminate(){
    //Will define closing of open resources eg sockets, file descriptors etc
    fprintf(stderr, "Exiting...\n");
    exit(-1);
  }

  static void sigHandler(int signalNum){
    switch(signalNum){
      case SIGINT:{
	terminate();
	break;
      }
      default:{
	fprintf(stderr, "\033[31mUnhandled signal %d\n\033[00m", signalNum);
	break;
      }
    }
  }

  void setSigHandler(){
    struct sigaction *theAction;
    theAction = (struct sigaction *)malloc(sizeof(struct sigaction));
    theAction->sa_handler = sigHandler;
    sigaction(SIGINT, theAction, NULL);
  }

  void sigchld_handler(int s){
    while(waitpid(-1, NULL, WNOHANG) > 0);
  }

  int runServer(const word port, FILE *ifp){
    if (ifp == NULL){
      raiseWarning("Null file pointer passed in");
      return -2;
    }

    int convertedFD = fileno(ifp); 

    int  sockfd, new_fd; //Listen on sock_fd, the new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;

    struct sigaction sa;
    Bool YES = True;

    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM; //Bi-directional server

    int addrResolve = getaddrinfo(NULL, port, &hints, &servinfo);
    if (addrResolve != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(addrResolve));
      return 1;
    }

    //Loop through all the results, binding to the first that accepts
    for(p = servinfo; p != NULL; p = p->ai_next) {
      if (
        (sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1
      ){
        perror("server: socket");
        continue;
      }

      //Enabling re-usability of our socket
      if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &YES, sizeof(int)) == -1){
        perror("setsockopt");
        return 1;
      }

      if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
        close(sockfd);
        perror("server: binding");
        continue;
      }

      break;
    }

    if (p == NULL) {
      fprintf(stderr, "server: failed to bind\n");
      return 2;
    }

    freeaddrinfo(servinfo);
    if (listen(sockfd, BACKLOG) == -1) {
      perror("listen");
      return 1;
    }

    sa.sa_handler = sigchld_handler;//Reaps all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
      perror("sigaction");
      return 1;
    }

    printf("\033[32mServer: serving on %s\033[00m\n", port);
    printf("Waiting for connections...\n");

    sin_size = sizeof(their_addr);
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

    if (new_fd == -1) {
      perror("accept");
    }

    char clientIP[INET6_ADDRSTRLEN];

    inet_ntop(
      their_addr.ss_family, 
      get_in_addr((struct sockaddr *)&their_addr), 
      clientIP, sizeof(clientIP)
    );

    printf("server: got connection from %s\n", clientIP);

    //Setting up variables for the transaction
    long long int totalSentByteCount = 0;
    char *sendBuf = NULL;

    //Setting up resources to poll for incoming data through input-terminal
    struct timeval timerStruct;
    timerStruct.tv_sec = POLL_TIMEOUT_SECS_SERVER;
    timerStruct.tv_usec = POLL_TIMEOUT_USECS_SERVER;

    fd_set descriptorSet;
    FD_ZERO(&descriptorSet);
    FD_SET(convertedFD,&descriptorSet);

    select(convertedFD+1, &descriptorSet, NULL, NULL, &timerStruct);

    //Let's modify the input terminals settings to match our specs 

    TermPair termPair;
    initTermPair(convertedFD, &termPair);
    //Changing the terminal's I/O speeds
    
    BaudRatePair baudP;
    initTBaudRatePair(&termPair, TARGET_BAUD_RATE, TARGET_BAUD_RATE); 

    if (setBaudRate(&termPair, baudP) != True){
      raiseWarning("Failed to change baud rate");
    }

    termPair.newTerm.c_lflag &= ~(ICANON | ECHO | ECHOE);
    termPair.newTerm.c_oflag &= ~OPOST;

    //Time to flush our settings to the file descriptor
    tcsetattr(convertedFD, TCSANOW, &(termPair.newTerm));
    
    while (1){
      sendBuf = (char *)malloc(sizeof(char)*BUF_SIZ);
      int nRead = 0;
      if (FD_ISSET(convertedFD, &descriptorSet)){ 
	//New data has come in the timer gets reset
        nRead = getChars(ifp, sendBuf, BUF_SIZ);
      }

    #ifdef DEBUG
      printf("rS %s\n", sendBuf);
    #endif

      if (! nRead){
      #ifdef DEBUG
        raiseWarning("Failed to read in a character from");
      #endif
      }

      int sentByteCount = send(new_fd, sendBuf, strlen(sendBuf), 0);

      if (sentByteCount == -1)  perror("send");
      else totalSentByteCount += sentByteCount;

      free(sendBuf);

      fprintf(stderr, "Total bytes sent: %lld\r", totalSentByteCount);
    }

    printf("Done reading\n");

    while (! feof(ifp)){
      sleep(1);
    }

    //Clean up here

    //Reverting the input terminal's settings
    tcsetattr(convertedFD, TCSANOW, &(termPair.origTerm));
    close(new_fd);

    return 0;
  }
#endif

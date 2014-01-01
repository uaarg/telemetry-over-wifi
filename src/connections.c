// Author: Emmanuel Odeke <odeke@ualberta.ca>
// Utility to enable connections and data passage depending on state:
//   ie Receiving vs Sending

#include <assert.h>
#include <signal.h> 
#include <stdio.h>
#include <string.h>

#include "../include/platformHandler.h"

#ifdef UNIX
  #include <termios.h>
#endif

#include "../include/ioLib.h"
#include "../include/errors.h"
#include "../include/cserial.h"
#include "../include/polling.h"
#include "../include/sigHandling.h"
#include "../include/dataTypes.h"
#include "../include/connections.h"
#include "../include/platformHandler.h" 
#include "../include/constants.h"

void *get_in_addr(struct sockaddr *sa){
  // Brian Beej showed me how to do this one
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void *socketViaStruct(void *pHStruct){
  portHostStruct pHSt = *(portHostStruct *)pHStruct;
  int *sockResult = (int *)malloc(sizeof(int));
  *sockResult = socketConnection(pHSt.hostName, pHSt.port);

  return (void *)sockResult;
}

int socketConnection(const word TARGET_HOST, const word PORT){
  if (TARGET_HOST == NULL) return ERROR_SOCKFD_VALUE;

  const short port = atoi(PORT);
  if (port < MIN_PORT_VALUE || port > MAX_PORT_VALUE) return ERROR_SOCKFD_VALUE;

  int sockfd = ERROR_SOCKFD_VALUE;
  char hostAddrString[INET6_ADDRSTRLEN];

  struct addrinfo hints, *servinfo, *p;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_UNSPEC; //IPv4/6 agnostic
  hints.ai_socktype = SOCK_DGRAM; // UDP

  int addrResolveResult = getaddrinfo(TARGET_HOST,PORT,&hints, &servinfo);
  if (addrResolveResult != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(addrResolveResult));
    return ERROR_SOCKFD_VALUE;
  }

  for(p = servinfo; p != NULL; p = p->ai_next){
    sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol);

    if (sockfd == ERROR_SOCKFD_VALUE){
      perror("Client socket binding");
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == ERROR_SOCKFD_VALUE) {
      close(sockfd);
      perror("Client socket connecting");
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return ERROR_SOCKFD_VALUE;
  }

  inet_ntop(
    p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), 
    hostAddrString, sizeof(hostAddrString)
  );

  freeaddrinfo(servinfo);
  printf("Client: connected to %s\n", hostAddrString);

  return sockfd;
}

void *msgTransit(void *data){
  fdPair *fdData = (fdPair *)data;

  if (fdData == NULL){
    raiseWarning("NULL fdData passed in"); // Fatality set to False
    return NULL;
  }

  DataState currentState = fdData->state;

  //long long int
  LLInt *transactionByteCount = (LLInt *)malloc(sizeof(LLInt));
  //**Remember to free the memory allocated in order to return the byte count

  *transactionByteCount = 0;

  //Setting up the socket-polling timer struct
  struct timeval transactionTimerSt;
  transactionTimerSt.tv_sec = POLL_TIMEOUT_SECS_CLIENT;
  transactionTimerSt.tv_usec = POLL_TIMEOUT_USECS_CLIENT;

  LLInt (*stateFunc)(fdPair *, struct timeval) = NULL;

  switch(currentState){
    case SENDING:{
      stateFunc = sendData;
      break;
    }

    case RECEIVING:{
      stateFunc = recvData;
      break;
    }

    default:{
      break;
    }
  }

  if (stateFunc != NULL) 
    *transactionByteCount = stateFunc(fdData, transactionTimerSt);


#ifdef DEBUG
  printf("byteTrans %lld\n", *transactionByteCount);
#endif

  return transactionByteCount;
}

LLInt sendData(fdPair *fDP, struct timeval timerStruct){
  //Setting up variables for the transaction
  LLInt totalSentByteCount = 0;

  int to = fDP->toFD;
  int from = fDP->fromFD;

  fd_set descriptorSet;
  FD_ZERO(&descriptorSet);
  FD_SET(from, &descriptorSet);


  unsigned int BUF_SIZ = fDP->bufSize;
 
  select(from+1, &descriptorSet, NULL, NULL, &timerStruct);
  int eofState = False; //Once set, EOF was encountered

  while (1) {
    word sendBuf = (word)malloc(sizeof(char)*BUF_SIZ);
    assert(sendBuf != NULL);

    int nRead = 0;
    char c;

    while (nRead < BUF_SIZ) {
      if (FD_ISSET(from, &descriptorSet)) { 
	int readResult = read(from, &c, 1);
	if (readResult == 0) { //EOF encountered
	  eofState = True;
	  break;
	} else if (readResult == -1) break; //A read error occured

	sendBuf[nRead] = c;
	++nRead;
      } else {
	continue;
      }
    }

    sendBuf[nRead] = '\0';

    //printf("%s",sendBuf);
    if ((nRead == 0) && (eofState == True)) {
      freeWord(sendBuf);
    #ifdef DEBUG
      printf("EOFFFFFFFFFFF HERE ");
    #endif
      break;
    }

  #ifdef DEBUG
    printf("%s", sendBuf);
  #endif

    if (! nRead){
    #ifdef DEBUG
      raiseWarning("Failed to read in a character from");
    #endif
    } else {
      int sentByteCount = send(to, sendBuf, nRead, 0);

      if (sentByteCount == -1)  perror("send");
      else totalSentByteCount += sentByteCount;
    }

    fprintf(
      stderr, "\033[3mTotal bytes sent: %lld\033[00m\r", totalSentByteCount
    );

    if (sendBuf != NULL) {
      freeWord(sendBuf);
    }

    //Finally after all required data has been sent, check to see if
    //our last read produced End of File (EOF)
    if (eofState == True) break;
  }

  printf("\n");
  clearCursorLine(stdout);

  printf("Done reading\n");  

  return totalSentByteCount;
}

LLInt recvData(fdPair *fDP, struct timeval tv){
  fd_set monitorFDS;

  int toFD = fDP->toFD;
  int sockfd = fDP->fromFD;

  FD_ZERO(&monitorFDS);
  FD_SET(sockfd,&monitorFDS);

  select(sockfd+1, &monitorFDS, NULL, NULL, &tv);

  int bufferedReads = 0;
  LLInt totalBytesIn=0, nRecvdBytes=0;

  while (1){
    if (FD_ISSET(sockfd, &monitorFDS)){
    #ifdef DEBUG
      fprintf(stderr, "Total bytes read in: %lld\r", totalBytesIn);
    #endif 
    }else{
      //printf("Monitorfd time out\n");
      continue;
    }

    char *buf = (char *)malloc(sizeof(char)*MAX_BUF_LENGTH);

    if (buf == NULL){
      fprintf(stderr, "Run out-of memory!!\n");
      exit(-1);
    }

    nRecvdBytes = recv(sockfd, buf, MAX_BUF_LENGTH-1, 0);

    Bool breakTrue = nRecvdBytes ? False : True;
    if (breakTrue){
      freeWord(buf);
      break;
    }
    
    ssize_t expectedWriteResult = strlen(buf); 
    if (write(toFD, buf, expectedWriteResult) != expectedWriteResult){
      raiseWarning("Write error");
    } 

    free(buf);
    ++bufferedReads;
    totalBytesIn += nRecvdBytes;
  }

  return totalBytesIn;
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
    sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol);
    if (sockfd == -1){
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
  LLInt totalRecvteCount = 0;

  //Setting up resources to poll for incoming data through input-terminal
  struct timeval timerStruct;
  timerStruct.tv_sec = POLL_TIMEOUT_SECS_SERVER;
  timerStruct.tv_usec = POLL_TIMEOUT_USECS_SERVER;

  fd_set descriptorSet;
  FD_ZERO(&descriptorSet);
  FD_SET(convertedFD,&descriptorSet);

  select(convertedFD+1, &descriptorSet, NULL, NULL, &timerStruct);

  //Let's modify the input terminals settings to match our specs 

  // TermPair termPair;
  // initTermPair(new_fd, &termPair);
  // //Changing the terminal's I/O speeds
  
  // BaudRatePair baudP;
  // initTBaudRatePair(&termPair, TARGET_BAUD_RATE, TARGET_BAUD_RATE); 

  // if (setBaudRate(&termPair, baudP) != True) {
  //   raiseWarning("Failed to change baud rate");
  // }

  // termPair.newTerm.c_lflag &= ~(ICANON | ECHO | ECHOE);
  // termPair.newTerm.c_oflag &= ~OPOST;

  // //Time to flush our settings to the file descriptor
  // tcsetattr(new_fd, TCSANOW, &(termPair.newTerm));
  // tcflush(new_fd, TCOFLUSH);
    
  while (1) {
    while (FD_ISSET(new_fd, &descriptorSet)){ 
      //New data has come in the timer gets reset
      //nRead = getChars(convertedFD, sendBuf, BUF_SIZ);
      ;
    }

    word recvBuf = (word)malloc(sizeof(char)*MAX_BUF_LENGTH);
    int recvByteCount = recv(new_fd, recvBuf, MAX_BUF_LENGTH-1, 0);

    if (recvByteCount == 0){ //Peer has performed an orderly shutdown
      fflush(ifp);
      raise(SIGTERM); //Hacky way of closing down our processes, to be refined
    }

    else if (recvByteCount == -1){  
      perror("send");
    }else{ 
      totalRecvteCount += recvByteCount;
      fwrite(recvBuf, sizeof(char), recvByteCount, ifp);
      fflush(ifp);
    }

    freeWord(recvBuf);

    
    fprintf(stderr, "Total bytes recvd: %lld\r", totalRecvteCount);
  }

  close(new_fd);

  return 0;
}

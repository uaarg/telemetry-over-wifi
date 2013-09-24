#include "../include/platformHandler.h"
#include "../include/connections.h"
#include <assert.h>

void initBiSocket(BiSocket *sock){
  if (sock != NULL){
    memset(sock, ERROR_SOCKFD_VALUE, sizeof(sock));
  }
}  

void *get_in_addr(struct sockaddr *sa){
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int socketConnection(const word TARGET_HOST, const word PORT){
  if (TARGET_HOST == NULL) return ERROR_SOCKFD_VALUE;

  const short port = atoi(PORT);
  if (port < MIN_PORT_VALUE || port > MAX_PORT_VALUE) return ERROR_SOCKFD_VALUE;

  int sockfd = ERROR_SOCKFD_VALUE;
  int addrResolveResult;

  struct addrinfo hints, *servinfo, *p;

  char hostAddrString[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_UNSPEC; //IPv4/6 agnostic
  hints.ai_socktype = SOCK_STREAM; //Bi-directional

  addrResolveResult=getaddrinfo(TARGET_HOST,PORT,&hints, &servinfo);
  if (addrResolveResult != 0){
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
    raiseWarning("NULL fdData passed in");
    return NULL;
  }

  DataState currentState = fdData->state;
  long long int byteTransaction = 0;

  switch(currentState){
    case SENDING:{
      struct timeval sendingTimerStruct;
      sendingTimerStruct.tv_sec = POLL_TIMEOUT_SECS_CLIENT;
      sendingTimerStruct.tv_usec = POLL_TIMEOUT_USECS_CLIENT;
      byteTransaction = sendData(fdData, sendingTimerStruct);
      break;
    }

    case RECEIVING:{
      struct timeval recvTimerStruct;
      recvTimerStruct.tv_sec = POLL_TIMEOUT_SECS_CLIENT;
      recvTimerStruct.tv_usec = POLL_TIMEOUT_USECS_CLIENT;
      byteTransaction = recvData(fdData, recvTimerStruct);
      break;
    }

    default:{
      break;
    }
  }

  printf("byteTrans %lld\n", byteTransaction);
  return NULL;
}

long long int sendData(fdPair *fDP, struct timeval timerStruct){
  //Setting up variables for the transaction
  long long int totalSentByteCount = 0;

  int to = fDP->toFD;
  int from = fDP->fromFD;

  fd_set descriptorSet;
  FD_ZERO(&descriptorSet);
  FD_SET(from, &descriptorSet);

  select(from+1, &descriptorSet, NULL, NULL, &timerStruct);

  //Let's modify the input terminals settings to match our specs 

  TermPair termPair;
  initTermPair(from, &termPair);
  //Changing the terminal's I/O speeds
  initTBaudRatePair(&termPair, TARGET_BAUD_RATE, TARGET_BAUD_RATE); 

  termPair.newTerm.c_lflag &= ~(ICANON | ECHO | ECHOE);
  termPair.newTerm.c_oflag &= ~OPOST;

  //Time to flush our settings to the file descriptor
  tcsetattr(from, TCSANOW, &(termPair.newTerm));

  unsigned int BUF_SIZ = fDP->bufSize;
 
  int eofState = False; //Once set, EOF was encountered
  while (1){
    word sendBuf = (word)malloc(sizeof(char)*BUF_SIZ);
    assert(sendBuf);

    int nRead = 0;
    if (FD_ISSET(from, &descriptorSet)){ 
      //New data has come in the timer gets reset
      nRead = getChars(from, sendBuf, BUF_SIZ, &eofState);
      if ((nRead == 0) && (eofState == True)){
	if (sendBuf != NULL) freeWord(sendBuf);
	printf("EOFFFFFFFFFFF HERE ");
	break;
      }
    }

    #ifdef DEBUG
      printf("%s", sendBuf);
    #endif

    if (! nRead){
    #ifdef DEBUG
      raiseWarning("Failed to read in a character from");
    #endif
    }else {
      int sentByteCount = send(to, sendBuf, nRead, 0);

      if (sentByteCount == -1)  perror("send");
      else totalSentByteCount += sentByteCount;
    }

    //Finally after all required data has been sent, check to see if
    //our last read produced End of File (EOF)
    fprintf(
      stderr, "\033[3mTotal bytes sent: %lld\033[00m\r", totalSentByteCount
    );

    if (sendBuf != NULL) freeWord(sendBuf);
    if (eofState == True) break;
  }

  printf("Done reading\n");

  //Clean up here

  //Reverting the input terminal's settings
  tcsetattr(from, TCSANOW, &(termPair.origTerm));

  return totalSentByteCount;
}

long long int recvData(fdPair *fDP, struct timeval tv){
  fd_set monitorFDS;

  int toFD = fDP->toFD;
  int sockfd = fDP->fromFD;

  FD_ZERO(&monitorFDS);
  FD_SET(sockfd,&monitorFDS);

  select(sockfd+1, &monitorFDS, NULL, NULL, &tv);

  int bufferedReads = 0;
  long long int totalBytesIn=0, nRecvdBytes=0;

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

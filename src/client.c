/*
  Author: Emmanuel Odeke <odeke@ualberta.ca>
  Client: sample usage:
    echo "Aloha ola and bonjour" | ./client localhost 5000
    ./client localhost 5000 src/foo.txt
    cat src/\*.c | ./client localhost 8080 
*/
#include <assert.h>
#include <pthread.h>

#include "../include/ioLib.h"
#include "../include/cserial.h"
#include "../include/polling.h"
#include "../include/connections.h"
#include "../include/platformHandler.h"

#define USAGE_CLIENT \
  "Usage:: \033[33m./client <hostname> <port> <serialdevice> <baudrate>\n\t\033[32m\
    Reads implicitly from standard input thus allows piping\033[00m\n"

int main(int argc, char *argv[]){

  if (argc < 3) {
    fprintf(stderr, USAGE_CLIENT);
    exit(1);
  }

  int infd = 0; //By default read from standard input unless otherwise specified
  int targetBaudRate_int = 57600; //Default target baud rate

  if (argc > 3) {

    if (argc > 4) {
      targetBaudRate_int = atoi(argv[4]);
    }

    const word srcPath = argv[3];
  #ifdef DEBUG
    printf("Source file: %s\n", srcPath);
  #endif

    infd = c_init_serial(srcPath, targetBaudRate_int, False);

    if (infd == ERROR_SOCKFD_VALUE) {
      raiseWarning("Error opening input device");
      exit(-1);
    }
  } else {
    infd = c_init_serialFD(infd, targetBaudRate_int, False);
  }

  word hostname = argv[1];
  word port = argv[2];

  pollThStruct pollTST;
  initPollThStruct(
    &pollTST, POLL_TIMEOUT_SINGLE_SEC, (void *)&ERROR_SOCKFD_VALUE, intPtrComp
  );
  pollTST.funcToRun = socketViaStruct;
 
  portHostStruct pHStruct; 
  pHStruct.hostName = hostname;
  pHStruct.port = port;

  pollTST.arg = &pHStruct;

  pollTill(&pollTST);

  int *sockfd = (int *)pollTST.savSuccess;

#ifdef DEBUG
  printf("socKetFileDescriptor value %d\n", *sockfd);
#endif
  if (*sockfd == ERROR_SOCKFD_VALUE){
    if (sockfd != NULL) free(sockfd);
    raiseWarning("Error while opening socket");
    exit(-1);
  }

  /*
  //Uncomment to enable receiving mode
  pthread_t receiveThread;
  fileDescriptorPair recvfDPair;
  recvfDPair.fromFD = sockfd;
  recvfDPair.toFD = infd;
  recvfDPair.state = RECEIVING;
  pthread_create(&recvThread, NULL, msgTransit, &recvfDPair);
  //pthread_join(recvThread, NULL);
  */

  pthread_t sendThread; 

  fdPair sendFilePair; 
  sendFilePair.fromFD = infd;
  sendFilePair.toFD = *sockfd;
  sendFilePair.state = SENDING;
  sendFilePair.bufSize = MAX_BUF_LENGTH;

  pthread_create(&sendThread, NULL, msgTransit, &sendFilePair);

  LLInt *totalTransactionCount = NULL;

  //Let's get back the number of bytes transferred
  pthread_join(sendThread, (void *)totalTransactionCount);

#ifdef DEBUG_STATISTICS
  printf("Total bytes sent: %lld\n", totalTransactionCount);
#endif

  if (totalTransactionCount != NULL) free(totalTransactionCount);

  if (infd != ERROR_SOCKFD_VALUE) 
    close(infd);

  close(*sockfd);

  if (sockfd != NULL) free(sockfd);

  return 0;
}

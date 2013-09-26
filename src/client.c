/*
  Author: Emmanuel Odeke <odeke@ualberta.ca>
  Client: sample usage:
*/
#include <assert.h>
#include <pthread.h>

#include "../include/ioLib.h"
#include "../include/polling.h"
#include "../include/connections.h"
#include "../include/platformHandler.h"

int main(int argc, char *argv[]){
  if (argc < 4) {
    fprintf(stderr,"usage: ./client <hostname> <port> <inputFile>\n");
    exit(1);
  }

  FILE *inFilePointer = fopen(argv[3], "r");
  if (inFilePointer == NULL) {
    fprintf(stderr, "Error reading input file\n");
    exit(-1);
  }
  int infd = fileno(inFilePointer);

  word hostname = argv[1];
  word port = argv[2];

  pollThStruct pollTST;
  initPollThStruct(&pollTST, 1, (void *)&ERROR_SOCKFD_VALUE, intPtrComp);
 
  portHostStruct pHStruct; 
  pHStruct.hostName = hostname;
  pHStruct.port = port;

  pollTST.arg = &pHStruct;
  pollTST.funcToRun = socketViaStruct;

  pollTill(&pollTST);

  int *sockfd = (int *)pollTST.savSuccess;

  printf("socKFD %d\n", *sockfd);
  if (*sockfd == ERROR_SOCKFD_VALUE){
    if (sockfd != NULL) free(sockfd);
    fprintf(stderr, "Error while opening socket\n");
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

  if (totalTransactionCount != NULL) free(totalTransactionCount);

  fflush(inFilePointer);
  fclose(inFilePointer);
  close(*sockfd);

  //if (sockfd != NULL) free(sockfd);

  return 0;
}

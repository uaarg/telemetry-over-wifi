/*
  Author: Emmanuel Odeke <odeke@ualberta.ca>
  Client: sample usage:
*/
#include <assert.h>
#include <pthread.h>

#include "../include/ioLib.h"
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

  int sockfd = socketConnection(hostname, port);
  if (sockfd == ERROR_SOCKFD_VALUE){
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
  sendFilePair.toFD = sockfd;
  sendFilePair.fromFD = infd;
  sendFilePair.state = SENDING;
  sendFilePair.bufSize = MAX_BUF_LENGTH;
  pthread_create(&sendThread, NULL, msgTransit, &sendFilePair);

  LLInt *totalTransactionCount = NULL;
  //Let's get back the number of bytes transferred
  pthread_join(sendThread, (void *)totalTransactionCount);

  if (totalTransactionCount != NULL) free(totalTransactionCount);

  fflush(inFilePointer);
  fclose(inFilePointer);
  close(sockfd);

  return 0;
}

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

  if (argc < 3) {
    fprintf(stderr,"usage: ./client <hostname> <port>\n");
    exit(1);
  }

  FILE *outFP = stdout;//By default, we write to standard output

  //Otherwise, we have an alternative outfile
  if (argc == 4){ 
    outFP = fopen(argv[3], "r+w");
    assert(outFP != NULL);
  }

  word hostName = argv[1];
  word port = argv[2];

  int sockfd; 

  sockfd = socketConnection(hostName, port);
  if (sockfd == ERROR_SOCKFD_VALUE){
    fprintf(stderr, "Connection failed\n");
    exit(-1);
  }

  //Setting up infrastructure to poll the data source socket
  //struct timeval tv;

  pthread_t recvTh;
  fdPair recvfDPair;
  int outfd = fileno(outFP);
  recvfDPair.fromFD = sockfd;
  recvfDPair.toFD = outfd;
  recvfDPair.state = RECEIVING;
  pthread_create(&recvTh, NULL, msgTransit, &recvfDPair);
  //pthread_join(recvTh, NULL);

  //pthread_t sendTh; 
  //fdPair sendfDPair; 
  //sendfDPair.toFD = sockfd;
  //sendfDPair.fromFD = outfd;
  //sendfDPair.state = SENDING;
  //pthread_create(&sendTh, NULL, msgTransit, &sendfDPair);

  while (1){
    sleep(1);
  }

  fflush(outFP);
  fclose(outFP);
  close(sockfd);
  return 0;
}

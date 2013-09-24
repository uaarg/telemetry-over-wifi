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
    fprintf(stderr,"usage: ./client <hostname> <port> [<dataSourcePath>]\n");
    exit(1);
  }

  FILE *outFP = stdout;//By default, we'll read from standard output

  //Otherwise, we have an alternative infile for reading and writing
  if (argc == 4){ 
    outFP = fopen(argv[3], "r+w");
    if (outFP == NULL){ 
      //The requested file doesn't exist so we'll create one
      outFP = fopen(argv[3], "w");
      if (outFP == NULL){
       //Last line of defense either permission problems or 
       //too many files got created
       fprintf(
	stderr, "\033[31mCould not create file %s\n%s\033\n[00m", 
	argv[3], FILE_CREATION_FAILURE_MSG 
       );
       exit(-1);
      }
    }
  }

  word hostName = argv[1];
  word port = argv[2];

  int sockfd; 

  sockfd = socketConnection(hostName, port);
  if (sockfd == ERROR_SOCKFD_VALUE){
    fprintf(stderr, "Connection failed\n");
    exit(-1);
  }

  int outfd = fileno(outFP);

  /*
  //Uncomment to enable receiving mode
  pthread_t recvTh;
  fdPair recvfDPair;
  recvfDPair.fromFD = sockfd;
  recvfDPair.toFD = outfd;
  recvfDPair.state = RECEIVING;
  pthread_create(&recvTh, NULL, msgTransit, &recvfDPair);
  //pthread_join(recvTh, NULL);
  */

  pthread_t sendTh; 
  fdPair sendfDPair; 
  sendfDPair.toFD = sockfd;
  sendfDPair.fromFD = outfd;
  sendfDPair.state = SENDING;
  sendfDPair.bufSize = MAX_BUF_LENGTH;
  pthread_create(&sendTh, NULL, msgTransit, &sendfDPair);
  pthread_join(sendTh, NULL);

  fflush(outFP);
  fclose(outFP);
  close(sockfd);
  return 0;
}

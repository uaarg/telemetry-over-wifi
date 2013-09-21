/*
  Author: Emmanuel Odeke <odeke@ualberta.ca>
  Client: sample usage:
*/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "platformHandler.h"
#define MAX_BUF_LENGTH 100 //Max number of bytes we can get at once

int POLL_TIMEOUT_SECS = 2;
int POLL_TIMEOUT_USECS = 500000;

//Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[]){

  if (argc < 3) {
    fprintf(stderr,"usage: ./client <hostname> <port>\n");
    exit(1);
  }

  FILE *outFP = stdout;//By default, we write to standard output

  //Otherwise, we have an alternative outfile
  if (argc == 4){ 
    outFP = fopen(argv[3], "a");
    assert(outFP != NULL);
  }

  int sockfd, 
      nRecvdBytes, 
      addrResolveResult;

  struct addrinfo hints, 
	 *servinfo, *p;

  char s[INET6_ADDRSTRLEN];

  char *host = argv[1];
  char *port = argv[2];

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_UNSPEC; //IPv4/6 agnostic
  hints.ai_socktype = SOCK_STREAM; //Bi-directional

  if ((addrResolveResult = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(addrResolveResult));
    return 1;
  }

  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
      perror("Client failed to bind a socket");
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("Client failed to connect through socket");
      continue;
    }
    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return 2;
  }

  inet_ntop(
    p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof(s)
  );

  freeaddrinfo(servinfo);

  printf("client: connecting to %s\n", s);

  //Setting up infrastructure to poll the data source socket
  struct timeval tv;
  fd_set monitorFDS;

  tv.tv_sec = POLL_TIMEOUT_SECS;
  tv.tv_usec = POLL_TIMEOUT_USECS;

  FD_ZERO(&monitorFDS);
  FD_SET(sockfd,&monitorFDS);

  select(sockfd+1, &monitorFDS, NULL, NULL, &tv);

  int bufferedReads = 0;
  long long int totalBytesIn=0;

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

    int breakTrue = nRecvdBytes ? 0 : 1;
    if (breakTrue){
      free(buf);
      break;
    }
    
    fprintf(outFP, "%s", buf);
    fflush(outFP);
    free(buf);
    ++bufferedReads;
    totalBytesIn += nRecvdBytes;
  }

  fflush(outFP);
  fclose(outFP);
  close(sockfd);
  return 0;
}

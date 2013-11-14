#include "../include/sigHandling.h"
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h>
#include <signal.h> 
#include <errno.h>

void terminate(){
  //Will define closing of open resources eg sockets, file descriptors etc
  fprintf(stderr, "Exiting...\n");
  exit(-1);
}

void shutDown(){
  //Will define proper shutdown of resources
  fprintf(stderr, "Shutting down...\n");
  exit(0);
}

void sigHandler(const int signalNum){
  switch(signalNum){
    case SIGINT:{
	terminate();
	break;
    }
    case SIGTERM:{
	shutDown();
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

void sigchld_handler(const int s){
  while(waitpid(-1, NULL, WNOHANG) > 0);
}

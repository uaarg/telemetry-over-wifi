#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h>
#include <signal.h> 
#include <errno.h>

#include "../include/SList.h"
#include "../include/errors.h"
#include "../include/dataTypes.h"
#include "../include/sigHandling.h"

// The list that keeps pointers to open files, allocated memory etc
// That will be properly freed / closed upon termination
SList *resourcesList = NULL;

void restoreTerm(void *termStorage) {
  printf("\033[32mRestoring terminal settings\033[00m\n %p", termStorage);

  if (termStorage != NULL) {
    TermPair *tp = (TermPair *)termStorage;
    if (tp->fd == -1) {
      raiseWarning("File fd wasn't opened well");
    } else {
      tcsetattr(tp->fd, TCSANOW, &(tp->origTerm));
      tcflush(tp->fd, TCOFLUSH);
    }
  } else {
    raiseWarning("termStorage is NULL");
  }
}

void closeAndFreeFP(void *fd) {
  if (fd != NULL) {
    close(*((int *)fd));
    free(fd);
  } else {
    raiseWarning("File descriptor to be closed cannot be NULL");
  }
}

void terminate(){
  //Will define closing of open resources eg sockets, file descriptors etc
  freeSList(resourcesList);

  fprintf(stderr, "Exiting...\n");
  exit(-1);
}

void shutDown(){
  fprintf(stderr, "Shutting down...\n");

  // Cleanup now
  freeSList(resourcesList);
  exit(0);
}

void sigHandler(const int signalNum) {
  switch(signalNum) {
    case SIGINT: {
	terminate();
	break;
    }
    case SIGTERM: {
	shutDown();
	break;
    }
	
    default: {
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

  // Initializing the file handle and resource tracking SList
  resourcesList = createSList();

  initSList(resourcesList, pseudoArgPass, restoreTerm, freeNode); 
}

int addToTrackedResources(void *termToTrack) {
  int addStatus = addToList(resourcesList, termToTrack); 
  printf("Add Status :%d\n", addStatus);
  return addStatus;
}

void sigchld_handler(const int s){
  while(waitpid(-1, NULL, WNOHANG) > 0);
}

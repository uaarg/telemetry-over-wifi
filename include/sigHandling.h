#ifndef _SIG_HANDLING_H
#define _SIG_HANDLING_H
  #include "SList.h"

  // Closes and then frees the memory assigned to a file descriptor
  void closeAndFreeFP(void *fd);

  void terminate();

  void setSigHandler();

  void sigchld_handler(const int);

  void shutDown();

  void sigHandler(const int);

  int addToTrackedResources(void *);

  void restoreTerm(void *termStorage);
#endif

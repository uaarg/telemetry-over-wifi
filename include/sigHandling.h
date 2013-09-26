#ifndef _SIG_HANDLING_H
#define _SIG_HANDLING_H
  void terminate();

  void setSigHandler();

  void sigchld_handler(const int);

  void shutDown();

  void sigHandler(const int);
#endif

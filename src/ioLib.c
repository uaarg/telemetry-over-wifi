#include <errno.h>
#include <stdio.h>
#include <signal.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/ioLib.h"
#include "../include/constants.h"

int getChars(FILE *fp, word destStr, const int len){
  if (destStr == NULL){
    raiseWarning("NULL string storage passed in.");
    return -1;
  }

  else if (fp == NULL){
    raiseWarning("NULL file pointer cannot be read from.");
    return -1;
  }

  char c;
  int nAdded = 0;
  for (nAdded=1; nAdded <= len; ++nAdded){
    c = getc(fp);
    if (c == EOF) break;
    destStr[nAdded-1] = c;
  }

  return nAdded;
}

Bool initTBaudRatePair(
  TermPair *tP, const speed_t inBaudRate, const speed_t outBaudRate 
){
  return initBaudRatePair(&(tP->baudRatePair), inBaudRate, outBaudRate); 
}

Bool flushTermSettings(TermPair *tP){
  if (tP == NULL) return False;

  if (tP->fd == ERROR_SOCKFD_VALUE) return False;
  tcsetattr(tP->fd, TCSANOW, &(tP->newTerm));

  return True;
}

Bool setBaudRate(TermPair *tP, const BaudRatePair newBP){
  if (tP == NULL || tP->fd == ERROR_SOCKFD_VALUE) return False;

  if (newBP.inBaudRate != ERROR_BAUD_RATE)
    cfsetispeed(&(tP->newTerm), newBP.inBaudRate);

  if (newBP.outBaudRate != ERROR_BAUD_RATE)
    cfsetospeed(&(tP->newTerm), newBP.outBaudRate);

  tcsetattr(tP->fd, TCSANOW, &(tP->newTerm));
  return True;
}

Bool revertTermSettings(TermPair *tP){
  if (tP == NULL || tP->fd == ERROR_SOCKFD_VALUE) return False;
  return tcsetattr(tP->fd, TCSANOW, &(tP->origTerm)) != ERROR_SETATTR_RESULT;
}

void initTermPair(const int fd, TermPair *tP){
  if (fd != ERROR_SOCKFD_VALUE && tP != NULL){
    tcgetattr(fd, &(tP->origTerm));
    tcgetattr(fd, &(tP->newTerm));
    tP->fd = fd;
  }
}

Bool initBaudRatePair(
  BaudRatePair *bP, const speed_t inSpeed, const speed_t outSpeed
){
  if (bP == NULL) return False;
  bP->inBaudRate = inSpeed;
  bP->outBaudRate = outSpeed;

  return True;
}

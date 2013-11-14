#include <errno.h>
#include <stdio.h>
#include <signal.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/ioLib.h"
#include "../include/errors.h"
#include "../include/constants.h"

void clearCursorLine(FILE *screen){
  FILE *targetScreen = screen;
  if (targetScreen == NULL)
    targetScreen = stdout;

  fprintf(targetScreen, "%c[2K\n", 27);
  fflush(targetScreen);
}

int getChars(int fd, word destStr, const int len, int *eofState){
  if (destStr == NULL){
    raiseError("NULL string storage passed in.", False); // Non-fatal err
    return -1;
  }

  
  if (fd == ERROR_SOCKFD_VALUE){
    raiseError("NULL file descriptor cannot be read from.", False);
    return -1;
  }

  char c;
  int nAdded = 0;

  for (; nAdded < len; ++nAdded){
    int readResult =read(fd, &c, 1);
    if (readResult == 0){//EOF encountered
      *eofState = True;
      break;
    }else if (readResult == -1) break; //A read error occured

    destStr[nAdded] = c;
  }

  if (nAdded) destStr[nAdded] = '\0';
  return nAdded;
}

Bool freeWord(word w){
#ifdef DEBUG
  printf("%s\n",__func__);
#endif
  if (w == NULL) return False;

  free(w);
  w = NULL;
  return True;
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

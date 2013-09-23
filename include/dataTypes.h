#ifndef _DATA_TYPES_H
#define _DATA_TYPES_H
  #include <stdio.h>
  #include <termios.h>

  typedef enum{
    False=0, True=1
  }Bool;

  typedef enum{
    INT_FD, FILE_FD
  }DescriptorState;  

  typedef enum{
    INVALID, SENDING, RECEIVING
  }DataState;

  typedef char *word;

  typedef struct{
    int sockFD;
  }BiSocket;

  typedef struct{
    int fromFD;
    int toFD;
    unsigned int bufSize;
    DataState state;
  }fdPair;
  
  typedef struct{
    speed_t inBaudRate;
    speed_t outBaudRate;
  }BaudRatePair;

  typedef struct{
    struct termios origTerm;
    struct termios newTerm;
    BaudRatePair baudRatePair;
    int fd;
  }TermPair;
#endif

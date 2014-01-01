// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _DATA_TYPES_H
#define _DATA_TYPES_H
  #include <stdio.h>
  #include <termios.h>

  typedef unsigned int uint32;

  typedef long long int LLInt;

  typedef enum {
    LT=-1, EQ=0, GT=1
  } Comparison;

  typedef enum {
    False=0, True=1
  } Bool;

  typedef enum {
    INT_FD, FILE_FD
  } DescriptorState;  

  typedef enum {
    INVALID, SENDING, RECEIVING
  } DataState;

  typedef char *word;

  typedef struct {
    int fromFD;
    int toFD;
    unsigned int bufSize;
    DataState state;
  } fdPair;
  
  typedef struct {
    speed_t inBaudRate;
    speed_t outBaudRate;
  } BaudRatePair;

  typedef struct {
    struct termios origTerm;
    struct termios newTerm;
    BaudRatePair baudRatePair;
    int fd;
  } TermPair;

  //Structure to enable polling and retrying of functions
  typedef struct {
    void *arg;
    void *errorValue;
    void *savSuccess; //Storage for success value
    double timeOut;
    uint32 tryCount;
    void *(*funcToRun)(void *data);
    Comparison (*valComp)(void *, void *);
  } pollThStruct;

  typedef struct {
    word port;
    word hostName;
  } portHostStruct;
#endif

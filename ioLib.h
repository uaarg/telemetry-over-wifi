/*
  Author: Emmanuel Odeke <odeke@ualberta.ca
  IOLib v1.0
*/
#ifndef _IO_LIB_H
#define _IO_LIB_H
  #include "dataTypes.h"

  #define raiseWarning(warning){\
    fprintf(stderr,\
      "\033[31mWarning %s on line %d in function \"%s\" in file %s\n\033[00m",\
      warning, __LINE__, __func__, __FILE__\
    );\
  }

  //Load into argument 2, a character sequence/word, at most n characters
  //read from the file pointer
  int getChars(FILE *, word , const int);

  //Given the input and output baud rates as the arguments,
  //copy their values and save them in the 'BaudRatePair'
  Bool initBaudRatePair(BaudRatePair *, const speed_t, const speed_t);
  
  //Copy to the 'BaudRatePair' contained in the TermPair, 
  //the input and output baud rates passed in as arguments 
  Bool initTBaudRatePair(TermPair *, const speed_t, const speed_t);

  //Copy into 'newTerm', the baud rates(input, and output) 
  //in the 'BaudRatePair' 
  Bool setBaudRate(TermPair *, const BaudRatePair);
  
  //Register/Flush to the file descriptor the current settings contained
  //in the 'newTerm' attribute of the TermPair argument. Returns True on success
  Bool flushTermSettings(TermPair *);

  //Flush to the file descriptor it's original settings saved in
  //attribute 'origTerm' in the 'TermPair'. Returns True on success
  Bool revertTermSettings(TermPair *);

  //Save the attributes of the file descriptor into the termPair
  //Attribute: origTerm will be untouched after first init, only used to 
  //revert the file descriptor's settings. newTerm will be the volatile 
  //attribute storing any changes made.
  void initTermPair(const int, TermPair *);

  //Handler for outgoing data to be pushed to the connected end of
  //a client socket
  void *outMsgHandler(void *);
  
  //Handler for incoming data pushed into a client socket
  void *inMsgHandler(void *);

  //Thread-based function to manage sending and receiving of data
  //based off the states 'RECEIVING' and 'SENDING'
  void *msgTransit(void *);
#endif

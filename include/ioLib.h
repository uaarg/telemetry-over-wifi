/*
  Author: Emmanuel Odeke <odeke@ualberta.ca
  IOLib v1.0
*/
#ifndef _IO_LIB_H
#define _IO_LIB_H
  #include "dataTypes.h"

  //Clears the line that the head of the file's reader is currently positioned
  //Performs a flush operation on the file after the clearing
  void clearCursorLine(FILE *);

  //Load into argument 2, a character sequence/char *, at most n characters
  //read from the file descriptor , argument 4 captures the state of reading ie
  //whether EOF has been encountered or not
  int getChars(int, char *, const int, int *);

  //Returns True if the argument to be free is not NULL, hence 
  //can be freed False otherwise. Once freed, sets the arg to NULL
  Bool freeWord(char *);

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
#endif

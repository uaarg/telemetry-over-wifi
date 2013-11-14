#ifndef POLLING_H
#define POLLING_H
  // While the data returned by the function to poll is NULL, sleep for the
  // defined timeOut then tryagain, incrementing the tryCount by 1 each time
  void pollTill(pollThStruct *);

  // Initialize the pollThStruct with the timeOut, 
  // argument 3 is the expected error value initialize the rest
  // of the numeric attributes as well, as defined in the *.c file
  // Arg 4 is the comparator
  void initPollThStruct(
    pollThStruct *, double, void *, Comparison (*)(void *, void *)
  );

  // Sample comparison functor
  Comparison intPtrComp(void *a, void *b);
#endif

#include <stdio.h>
#include <unistd.h>

#include "../include/dataTypes.h"
#include "../include/constants.h"
#include "../include/ioLib.h"

void *dTest(void *data){
  int *tP = (int *)data;
  if (*tP > 0){
    --(*tP);
    return NULL;
  }
  return tP;
}

void initPollThStruct(
  pollThStruct *pTSt, double timeOut, 
  void *errorValue, Comparison comp(void *, void *)
){
  if (pTSt != NULL){
    pTSt->tryCount = INIT_TRY_COUNT;
    pTSt->timeOut = timeOut;
    pTSt->errorValue = errorValue;
    pTSt->valComp = comp;
  }
}

void pollTill(pollThStruct *pollTST){
  if (pollTST == NULL){
    raiseWarning("NULL pollThStruct passed in");
    return; 
  }

  const double timeOutSecs = pollTST->timeOut;
  while (1){
    pollTST->savSuccess = pollTST->funcToRun(pollTST->arg);
  #ifdef DEBUG
    printf("%d\n", *(int *)pollTST->savSuccess);
  #endif
    Comparison vComparison = pollTST->valComp(
      pollTST->savSuccess, pollTST->errorValue
    );
    if (vComparison != EQ){
      fprintf(stderr, "\033[33mDone Polling as condition met\n\033[00m");
      break;
    }else{
      clearCursorLine(stderr);
      fprintf(stderr, "Try Count: %d Sleeping for %1.2f seconds\r", 
	++(pollTST->tryCount), timeOutSecs);
    }

    sleep(timeOutSecs);
  }
}

Comparison intPtrComp(void *a, void *b){
  int *aInt = (int *)a;
  int *bInt = (int *)b;

#ifdef DEBUG
  printf("intComp a %d b %d\n", *aInt, *bInt);
#endif

  if (*aInt != *bInt){
    return *aInt < *bInt ? LT : GT;
  }
  
  return EQ;
}

#ifdef DEBUG
int main(){
  pollThStruct pollTST;
  initPollThStruct(&pollTST, 1, NULL);
  int testArg = 10;

  pollTST.funcToRun = dTest;
  pollTST.arg = &testArg;
  pollTill(pollTST);
  return 0;
}
#endif

// Author: Emmanuel Odeke <odeke@ualberta.ca>
// Singly linked list

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "../include/SList.h"
#include "../include/errors.h"
#include "../include/dataTypes.h"

#define DEBUG

SList *createSList(void) {
  SList *newSL = NULL; 
  if ((newSL = (SList *)malloc(sizeof(SList))) == NULL) {
    // Fatal-error detected
    raiseError(
      "Run-out of memory, please release or debug memory allocations"
    );
  }

  newSL->size = 0;
  newSL->head = newSL->tail = NULL;

  // Initializing the functions to NULL
  newSL->copyData = NULL;
  newSL->freeData = NULL;
  newSL->freeNode = NULL;

  return newSL;
}

int freeNode(Node *n, void (*freeData)(void *)) {
  int freeCount = 0;

  if (n != NULL) {

    void (*dataFreer)(void *) = freeData;

    // Scoping rules:: Local variable overrides the global
    if (n->freeFunc != NULL)
      dataFreer = n->freeFunc;	 

    if (dataFreer == NULL) {
      raiseWarning("DataFree function cannot be NULL");
    } else {
      while (n != NULL) {
	Node *tmp = n->next;
	dataFreer(n->data);

	free(n);

	n = tmp;
	++freeCount;
      }
    } 

  } else {
  #ifdef DEBUG
    raiseWarning("NULL node cannot be freed"); // Non-fatal
  #endif
  }

  return freeCount;
}

int freeSList(SList *sl) {
  int freeCount = 0;
  if (sl != NULL) {
    int nFrees = sl->freeNode(sl->head, sl->freeData);
  #ifdef DEBUG
    printf("nFrees: %d\n", nFrees);
  #endif
    if (nFrees != sl->size) {
      raiseWarning("Number of freed nodes is not equal to the SL size");
    }
    
    sl->size = 0;
    free(sl);

    sl = NULL;
  }

  return freeCount;
}

void initSList(
    SList *sl, void * (dataCopy)(void *), 
    void (*dataFree)(void *), int (*nodeFree)(Node *, void (*)(void *))
) {
  if (sl == NULL) {
    raiseWarning("NULL singly linked list passed in for initialization");
  } else {
    sl->size = 0;
    sl->copyData = dataCopy;
    sl->freeNode = nodeFree;
    sl->freeData = dataFree;
    sl->head = sl->tail = NULL;
  }
}

Node *createNode(void *data) {
  Node *newNode = NULL;
  if ((newNode = (Node *)malloc(sizeof(Node))) == NULL) {
    // Fatal-error, wish you a quick recovery
    raiseError(
      "Run-out of memory, please release or debug memory allocations"
    );
  }

  newNode->data = data;
  newNode->next = NULL;
  newNode->visits = 0;

  // Optional function to enable freeing of data
  newNode->freeFunc = NULL;

  return newNode;
}

void *pseudoArgPass(void *arg) {
  return arg;
}

void *copyDblPtr(void *data) {
  double *inew = (double *)malloc(sizeof(double));
  *inew = *(double *)data;

  return (void *)inew;
}

void *copyIntPtr(void *data) {
  int *inew = (int *)malloc(sizeof(int));
  *inew = *(int *)data;

  return (void *)inew;
}
/*
Node *addNode(Node *n, void *data) {
  // In this case we'll skip adding the optional data freeing function
  // localized to the newly created node
  return addNodeAndFunc(n, data, NULL, NULL);
}
*/

Node *addNodeAndFunc(
  Node *n, void *data, void *(dataCopier)(void *), 
  void (*dataFreer)(void *)
) {
  if (data == NULL) {
    return 0;
  }

#ifdef DEBUG
  printf("%s:: Data %p \n", __func__, data);
#endif

  void *dataCopy = dataCopier(data);
  Node *temp = createNode(dataCopy);
  temp->next = n;
  n = temp;

  n->dataCopier = dataCopier;
  n->freeFunc = dataFreer;
  return n;
}

int addToList(SList *sl, void *data) {
  // The last two arguments are set to NULL to allow using 
  // the copy and data freeing functions set in the SList itself
  // instead of custom ones for the node itself
  return addToListWithFuncs(sl, data, NULL, NULL);
}

int addToListWithFuncs(
  SList *sl, void *data, void * (*copier)(void *), 
  void (*dataFreer)(void *)
) {
  if (sl == NULL) {
    // Non-fatal err -- treated as a warning
    raiseWarning("NULL SL passed in. First create SL");
    return 0;
  } else {
    void * (*copyingFunc)(void *) = sl->copyData;
   
    // Scoping rules: The defined function takes higher precedence than the 
    // SL's own copier function
    if (copier != NULL) 
      copyingFunc = copier;

    if (copyingFunc == NULL) {
      raiseWarning(
	"copyData function of SL is NULL. Please initialize it first"
      );
      return 0;
    }

    if (sl->head == NULL) { // First time adding data to this SL
      sl->head = addNodeAndFunc(sl->head, data, copyingFunc, dataFreer);
      sl->tail = sl->head->next;
    } else {
      sl->tail = addNodeAndFunc(sl->tail, data, copyingFunc, dataFreer);
    }

    // Make sure we've always got the head linked to the tail
    if (sl->head->next != sl->tail) 
      sl->head->next = sl->tail;

    ++(sl->size);

    return 1;
  }
}



inline unsigned int getSize(SList *sl) {
  return (sl == NULL ? 0 : sl->size);
}

void *strCopier(void *data) {
  return (void *)strdup((char *)data);
}

inline void *getData(Node *n) {
  return n == NULL ? NULL : n->data;
}

inline void *peek(SList *sl) {
  return sl == NULL ? NULL : getData(sl->head);
}

void *pop(SList *sl) {
  if (! getSize(sl)) {
  #ifdef DEBUG
    raiseWarning("Cannot peek from a NULL sl");
  #endif
    return NULL;
  } else {
    Node *prevHead = sl->head;
    if (prevHead != NULL) {

      /*
      void * (*copyFunc)(void *) = sl->copyData;
      if (sl->head->dataCopier != NULL)
	copyFunc = prevHead->dataCopier;

      // void (*dataFreer)(void *) = prevHead->freeFunc;

      if (dataFreer == NULL) {
	raiseError("Data freeing function cannot be NULL");
      } else {
      */
      void *copiedData = prevHead->data;
      sl->head = prevHead->next;

      free(prevHead);
      --sl->size;
      return copiedData;
    } else {
    #ifdef DEBUG
      raiseWarning("Trying to pop from an empty node");
    #endif
      return NULL;
    }
  }
}

#ifdef SAMPLE_RUN
int main() {

  SList *sl = createSList();
  initSList(sl, copyIntPtr, free, freeNode); 
  /*
  int i;
  // Testing with integers 
  for (i=0; i < 10; ++i) {
    addToList(sl, &i);
  }

  // Testing with doubles
  double d;
  for (d=300.0; d > 200.0; d -= 0.5) {
    // printf("dd: %.2f\n", d);
    addToListWithFuncs(sl, &d, copyDblPtr, free);
  }
  */
  char *s = "ODEKE\0";
  char *msg = "This is a test! Count down is on\0";

  // Testing with these strings 
  addToListWithFuncs(sl, (void *)s, strCopier, free);
  int i;
  for (i=0; i < 5; ++i) {
    char *peek1 = peek(sl);
    printf("Peeked: %s\n", peek1);
  }

  void *popd1 = pop(sl);
  printf("Popped data %s\n", (char *)popd1);
  if (popd1 != NULL) free(popd1);

  addToListWithFuncs(sl, (void *)msg, strCopier, free);
  printf("SL size: %u\n", getSize(sl));

  for (i=0; i < 3; ++i) {
    char *peek2 = peek(sl);
    printf("Peeked2: %s\n", peek2);
    void *popd = pop(sl);
    if (popd != NULL) 
      free(popd);
  }

  addToListWithFuncs(sl, (void *)msg, strCopier, free);
  printf("SL size: %u\n", getSize(sl));

  void *popD = pop(sl);
  printf("Popd %s\n", (char *)popD); 
  free(popD);

  printf("SL size: %u\n", getSize(sl));

  freeSList(sl);

  return 0;
}
#endif

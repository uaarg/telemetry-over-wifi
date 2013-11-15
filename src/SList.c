// Author: Emmanuel Odeke <odeke@ualberta.ca>
// Singly linked list

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/SList.h"
#include "../include/errors.h"
#include "../include/dataTypes.h"

SList *createSList(void) {
  SList *newSL = NULL; 
  if ((newSL = (SList *)malloc(sizeof(SList))) == NULL) {
    // Fatal-error detected
    raiseError(
      "Run-out of memory, please release or debug memory allocations", True
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
  if (freeData == NULL) {
    raiseError("DataFree function cannot be NULL", False);
  } else {
    while (n != NULL) {
      Node *tmp = n->next;
      freeData(n->data);

      free(n);

      n = tmp;
      ++freeCount;
    }
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
      raiseError("Number of freed nodes is not equal to the SL size", False);
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
    raiseError("NULL singly linked list passed in for initialization", False);
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
      "Run-out of memory, please release or debug memory allocations", True
    );
  }

  newNode->data = data;
  newNode->next = NULL;
  newNode->visits = 0;

  return newNode;
}

void *copyIntPtr(void *data) {
  int *inew = (int *)malloc(sizeof(int));
  *inew = *(int *)data;

  return (void *)inew;
}

Node *addNode(Node *n, void *data) {
  if (data == NULL) {
    return 0;
  }

#ifdef DEBUG
  printf("%s:: Data %p \n", __func__, data);
#endif

  Node *temp = createNode(data);
  temp->next = n;
  n = temp;

  return n;
}

int addToList(SList *sl, void *data) {
  if (sl == NULL) {
    // Non-fatal err -- treated as a warning
    raiseError("NULL SL passed in. First create SL", False);
    return 0;
  } else {
    if (sl->copyData == NULL) {
      raiseError(
	"copyData function of SL is NULL. Please initialize it first", False
      );
      return 0;
    }

    if (sl->head == NULL) { // First time adding data to this SL
      sl->head = addNode(sl->head, sl->copyData(data));
      sl->tail = sl->head->next;
    } else {
      sl->tail = addNode(sl->tail, sl->copyData(data));
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

#ifdef SAMPLE_RUN
int main() {

  SList *sl = createSList();
  initSList(sl, copyIntPtr, free, freeNode); 

  int i;
  for (i=0; i < 10000000; ++i) {
    addToList(sl, &i);
  }

  printf("SL size: %u\n", getSize(sl));

  freeSList(sl);

  return 0;
}
#endif

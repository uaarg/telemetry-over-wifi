// Author: Emmanuel Odeke <odeke@ualberta.ca>
// Singly linked list

#ifndef _SLIST_H
#define _SLIST_H
  typedef struct Node_ {
    void *data;
    int visits;
    // Optional Custom function for freeing data
    // However, when set, it's presence overrides that of any 
    // globally provided function ie The normal scoping rules
    void (*freeFunc)(void *); 
    struct Node_ *next;
  } Node;

  typedef struct {
    int size;
    Node *head, *tail;
    void * (*copyData)(void *);
    void (*freeData)(void *);
    int (*freeNode)(Node *, void (*)(void *));
  } SList;

  // Creates a Node-worth of memory, and set's the node's data attribute
  // to point to the passed in argument. It also initializes the node's
  // next pointer to NULL, then returns the node.
  Node *createNode(void *data);

  void initSList(
    SList *sl, void * (dataCopy)(void *), void (*dataFree)(void *), 
    int (*nodeFree)(Node *, void (*dataFree)(void *))
  );

  inline unsigned int getSize(SList *sl);

  // Creates an SList-memory worth on the heap. Initializes the SList's
  // size to zero, set's it's head and tail to NULL, as well as its
  // member functions to NULL and returns the initialized SList
  SList *createSList(void);

  // Frees the SList and returns the count of freed nodes
  // It raises a warning if the number of freed nodes doesn't match the
  // the noted size of the list
  int freeSList(SList *sl);
 
  // Adds argument to the sl 
  int addToList(SList *sl, void *data);

  // Allows you to add any data type to the SList with a custom 
  // data copier as well as a custom data freeing function.
  int addToListWithFuncs(
    SList *sl, void *data, void * (*copier)(void *), void (*dataFreer)(void *)
  );

  // Adds an element with content data, as well the
  // optional localized data freeing function
  Node *addNodeAndFunc(Node *n, void *data, void (*dataFreer)(void *));

  // Adds an element with content data, but sets the optional localized
  // data freeing function to NULL
  Node *addNode(Node *n, void *data);

  int freeNode(Node *n, void (*freeData)(void *));

  // Miscellaneous section 
  void *copyIntPtr(void *);

  void *pseudoArgPass(void *arg);
#endif

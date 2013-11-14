#ifndef _ERRORS_H
#define _ERRORS_H
  #include <stdio.h>
  #include <stdlib.h>
  #define raiseError(msg, fatality) {\
    fprintf(stderr, "%s [%s::%d] \033[31m%s\033[00m\n", \
      __FILE__, __func__, __LINE__, #msg);\
    if (fatality) {\
      exit(-1);\
    }\
  }
#endif

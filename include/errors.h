// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _ERRORS_H
#define _ERRORS_H
  #include <stdio.h>
  #include <stdlib.h>

  #define warnAndCheckFatality(msg, fatality) {\
    fprintf(stderr, "%s [%s::%d] \033[31m%s\033[00m\n", \
      __FILE__, __func__, __LINE__, #msg);\
    if (fatality) {\
      exit(-1);\
    }\
  }

  #define raiseError(msg) warnAndCheckFatality(msg, True)
  #define raiseWarning(warning) warnAndCheckFatality(warning, False)
#endif

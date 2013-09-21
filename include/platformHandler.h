#ifndef _PLATFORM_HANDLER_H
#define _PLATFORM_HANDLER_H
  #ifdef WINDOWS
    #include <winsock.h>
    #include <winsock2.h>
    #include <windows.h>
  #elif defined(UNIX)
    #include <netdb.h>
    #include <sys/wait.h>
    #include <arpa/inet.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
  #endif
#endif

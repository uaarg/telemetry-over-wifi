/*
  Author: Emmanuel Odeke <odeke@ualberta.ca>
  streamServer.c
*/
#include <sys/stat.h>
#include "redServer.h"

#define USAGE_INFO "./streamer <port> <pathToDataSource>"

int main(int argc, char *args[]){
  if (argc != 3){
    printf("%s\n",USAGE_INFO);
    exit(1);
  }

  setSigHandler(); //fire up the signal handler

  int portNumber;
  if (sscanf(args[1],"%d",&portNumber) != 1){
    raiseWarning("Please enter an integer between [0,65536] for the port");
    exit(2);
  }
#ifdef DEBUG
  fprintf(stderr, "Port selected: %d\n", portNumber);
#endif
  if (portNumber > (1<<16)){
    fprintf(stderr, "%d is an invalid port number. Range is [0,65536]\n",
	portNumber);
    exit(3);
  }

  char *filePath = args[2];
  fprintf(stderr, "source file: %s\n",filePath);

  //Ensuring that directories aren't passed in
  struct stat statInfo;
  stat(filePath, &statInfo);
  if (S_ISDIR(statInfo.st_mode)){
    fprintf(stderr,"%s is a directory. Use files or pipes\n",filePath);
    exit(4);
  }

  FILE *ifp = fopen(filePath,"r");
  runServer(args[1],ifp);

  fclose(ifp);
  return 0;
}

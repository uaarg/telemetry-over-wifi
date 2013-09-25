/*
  Author: Emmanuel Odeke <odeke@ualberta.ca>
  streamServer.c
*/
#include <sys/stat.h>

#include <stdlib.h>
#include <stdio.h>
#include "../include/redServer.h"

#define USAGE_INFO "./server <port> <dataStoragePath>"

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
  if (portNumber > (1<<16)){
    fprintf(stderr, "%d is an invalid port number. Range is [0,65536]\n",
	portNumber);
    exit(3);
  }

  char *filePath = args[2];
  fprintf(stderr, "Outfile: %s\n",filePath);

  struct stat statInfo;
  //Ensuring that directories aren't passed in
  if ((stat(filePath, &statInfo) == 0) && S_ISDIR(statInfo.st_mode)){
    fprintf(stderr,"%s is a directory. Use files or pipes\n",filePath);
    exit(4);
  }

  char *targetPort = args[1];

  FILE *ofp = fopen(filePath,"w");
  runServer(targetPort, ofp);
  
  if (ofp != NULL) fclose(ofp);
  return 0;
}

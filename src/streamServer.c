#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "../include/errors.h"
#include "../include/connections.h"

#define USAGE_INFO "./server <port> <dataStoragePath>"

int main(int argc, char *args[]) {
  if (argc != 3) {
    printf("\033[32m%s\033[00m\n",USAGE_INFO);
    exit(1);
  }

  setSigHandler(); //fire up the signal handler

  int portNumber;
  if ((sscanf(args[1],"%d",&portNumber) != 1) || (portNumber > MAX_PORT_VALUE)) {
    // Fatal-error here
    raiseError("Please enter an integer between [0, 65536] for the port");
  }

  char *filePath = args[2];
#ifdef DEBUG
  fprintf(stderr, "Outfile: %s\n",filePath);
#endif

  struct stat statInfo;
  //Ensuring that directories aren't passed in
  if ((stat(filePath, &statInfo) == 0) && S_ISDIR(statInfo.st_mode)) {
    fprintf(stderr,"%s is a directory. Use files or pipes\n",filePath);
    exit(4);
  }

  char *targetPort = args[1];

  FILE *ofp = fopen(filePath,"w");
  runServer(targetPort, ofp);
  
  if (ofp != NULL) fclose(ofp);
  return 0;
}

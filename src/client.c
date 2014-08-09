#include <assert.h>
#include <pthread.h>

#include "../include/ioLib.h"
#include "../include/errors.h"
#include "../include/cserial.h"
#include "../include/polling.h"
#include "../include/sigHandling.h"
#include "../include/connections.h"
#include "../include/platformHandler.h"

#define USAGE_CLIENT \
  "Usage:: \033[33m./client <hostname> <port> <serialdevice> <baudrate>\n\t\033[32m\
    Reads implicitly from standard input thus allows piping\033[00m\n"

int main(int argc, char *argv[]) {

  if (argc < 3) {
    fprintf(stderr, USAGE_CLIENT);
    exit(1);
  }

  TermPair tp;
  tp.fd = 0; //By default read from standard input unless otherwise specified
  int targetBaudRate_int = 57600; //Default target baud rate

  // Checking if the port is valid
  unsigned int portValue;
  char *port = argv[2];

  if ((sscanf(port, "%d", &portValue) != 1) || (portValue > MAX_PORT_VALUE)) {
    raiseWarning("Port number should range from [0 to 65536]");
  }

  if (argc > 3) {

    if (argc > 4) {
      targetBaudRate_int = atoi(argv[4]);
    }

    const char *srcPath = argv[3];
  #ifdef DEBUG
    printf("Source file: %s\n", srcPath);
  #endif

    c_init_serial(&tp, srcPath, targetBaudRate_int, False);

    if (tp.fd == ERROR_SOCKFD_VALUE) {
      raiseError("Error opening input device");
    }

  } else {
    c_init_serialFD(&tp, targetBaudRate_int, False);
  }

  setSigHandler(); //fire up the signal handler

  if (addToTrackedResources((void *)&tp) != 1) {
    raiseError(
    "Cannot add the infile descriptor for tracking and proper signal handling"
    );
  }

  pollThStruct pollTST;
  initPollThStruct(
    &pollTST, POLL_TIMEOUT_SINGLE_SEC, (void *)&ERROR_SOCKFD_VALUE, intPtrComp
  );
  pollTST.funcToRun = socketViaStruct;
 
  portHostStruct pHStruct; 

  char *hostname = argv[1];
  pHStruct.hostName = hostname;
  pHStruct.port = port;

  pollTST.arg = &pHStruct;

  pollTill(&pollTST);

  int *sockfd = (int *)pollTST.savSuccess;

#ifdef DEBUG
  printf("socKetFileDescriptor value %d\n", *sockfd);
#endif
  if (*sockfd == ERROR_SOCKFD_VALUE) {
    if (sockfd != NULL) free(sockfd);
    raiseError("Error while opening socket");
  }

  /*
  //Uncomment to enable Receiving mode
  pthread_t receiveThread;
  fileDescriptorPair recvfDPair;
  recvfDPair.fromFD = sockfd;
  recvfDPair.toFD = infd;
  recvfDPair.state = Receiving;
  pthread_create(&recvThread, NULL, msgTransit, &recvfDPair);
  //pthread_join(recvThread, NULL);
  */

  pthread_t sendThread; 

  fdPair sendFilePair; 
  sendFilePair.fromFD = tp.fd;
  sendFilePair.toFD = *sockfd;
  sendFilePair.state = Sending;
  sendFilePair.bufSize = MAX_BUF_LENGTH;

  pthread_create(&sendThread, NULL, msgTransit, &sendFilePair);

  LLInt *totalTransactionCount = NULL;

  //Let's get back the number of bytes transferred
  pthread_join(sendThread, (void *)totalTransactionCount);

#ifdef DEBUG_STATISTICS
  printf("Total bytes sent: %lld\n", totalTransactionCount);
#endif

  if (totalTransactionCount != NULL) free(totalTransactionCount);

  if (tp.fd != ERROR_SOCKFD_VALUE) 
    close(tp.fd);

  close(*sockfd);

  if (sockfd != NULL) free(sockfd);

  return 0;
}

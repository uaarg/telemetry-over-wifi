#ifndef _CONSTANTS_H
#define _CONSTANTS_H
  static const unsigned int INIT_TRY_COUNT = 0;
  static const word FILE_CREATION_FAILURE_MSG = \
	"Please check your file permissions or check for the restrictions on \
	the number of files that can be opened at once";
	
  static const int BACKLOG = 20;//How many pending connections queue will hold
  static const int BUF_SIZ = 1024;//Arbitrary size here

  static const int MAX_BUF_LENGTH = 1024;//Max number of bytes we can get at once
  static const int MIN_PORT_VALUE = 0;
  static const int MAX_PORT_VALUE = 1<<16;

  static const int POLL_TIMEOUT_SECS_SERVER = 0;
  static const int POLL_TIMEOUT_USECS_SERVER = 1000;

  static const int POLL_TIMEOUT_SECS_CLIENT = 2;
  static const int POLL_TIMEOUT_USECS_CLIENT = 500000;

  static const int ERROR_SOCKFD_VALUE = -1;
  static const int ERROR_SETATTR_RESULT = -1;

  static const speed_t ERROR_BAUD_RATE = -1;
  static const speed_t TARGET_BAUD_RATE = B57600;
#endif

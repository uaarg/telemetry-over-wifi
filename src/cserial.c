/*
   $Id$
   Copyright (C) 2004 Pascal Brisset, Antoine Drouin
   Copyright (C) 2013 Stephen Dwyer, Emmanuel Odeke

 C handling serial ports based on pprz ocaml bindings.

 This file is part of paparazzi.

 paparazzi is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
 any later version.

 paparazzi is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with paparazzi; see the file COPYING.  If not, write to
 the Free Software Foundation, 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA.
*/

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/termios.h>

#include "../include/errors.h"
#include "../include/cserial.h"

/*
 **************************************************
 *   Open serial device for requested protocol	  *
 **************************************************
*/

speed_t matchBaudRate(const int speed) {
  speed_t theBaudRate;

  switch (speed) {
    case 0:
      theBaudRate = B0;
      break;

    case 50:
      theBaudRate = B50;
      break;

    case 75:
      theBaudRate = B75;
      break;

    case 110:
      theBaudRate = B110;
      break;

    case 134:
      theBaudRate = B134;
      break;

    case 150:
      theBaudRate = B150;
      break;

    case 200:
      theBaudRate = B200;
      break;

    case 300:
      theBaudRate = B300;
      break;

    case 600:
      theBaudRate = B600;
      break;

    case 1200:
      theBaudRate = B1200;
      break;

    case 1800:
      theBaudRate = B1800;
      break;

    case 2400:
      theBaudRate = B2400;
      break;

    case 4800:
      theBaudRate = B4800;
      break;

    case 9600:
      theBaudRate = B9600;
      break;

    case 19200:
      theBaudRate = B19200;
      break;

    case 38400:
      theBaudRate = B38400;
      break;

    case 57600:
      theBaudRate = B57600;
      break;

    case 115200:
      theBaudRate = B115200;
      break;

    case 230400:
      theBaudRate = B230400;
      break;

    default:
      printf("bad baudrate %d\n", speed);
      theBaudRate = -1;
  }

  return theBaudRate;
}

void c_init_serial(
  TermPair *termStorage, const char *devicePath, 
  const int speed, const Bool hw_flow_control
) {
  if (termStorage == NULL) {
    raiseError("The storage for the file descriptor must not be NULL", True);
  }

  termStorage->fd = open(devicePath, O_RDWR|O_NONBLOCK);
  c_init_serialFD(termStorage, speed, hw_flow_control);
}

void c_init_serialFD(
  TermPair *termP, const int speed, const Bool hw_flow_control
  ) {

  if (termP == NULL) {
    raiseError(
      "A non-NULL termios pair is needed to store the new term attributes", True
    );
  }

  speed_t selectedBaudRate = matchBaudRate(speed);

  if (termP->fd == -1) {
    perror("opening modem serial device : fd < 0");
  }

  if (tcgetattr(termP->fd, &(termP->origTerm))) {
  #ifdef DEBUG
    printf("Saved original modem serial device settings for later restoration\n");
  #endif
  }

  termP->newTerm = termP->origTerm;

  /* input modes */
  termP->newTerm.c_iflag &= \
    ~(IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK|ISTRIP|INLCR|IGNCR
      |ICRNL |IXON|IXANY|IXOFF|IMAXBEL);
  /* pas IGNCR sinon il vire les 0x0D */
  termP->newTerm.c_iflag |= BRKINT;

  /* output_flags */
  termP->newTerm.c_oflag  &=~(OPOST|ONLCR|OCRNL|ONOCR|ONLRET);

  /* control modes */
  if (hw_flow_control) {
    termP->newTerm.c_cflag &= ~(CSIZE|CSTOPB|CREAD|PARENB|PARODD|HUPCL|CLOCAL);
    termP->newTerm.c_cflag |= CREAD|CS8|CLOCAL|CRTSCTS;
  } else {
    termP->newTerm.c_cflag &= \
      ~(CSIZE|CSTOPB|CREAD|PARENB|PARODD|HUPCL|CLOCAL|CRTSCTS);

    termP->newTerm.c_cflag |= CREAD|CS8|CLOCAL;
  }

  /* local modes */
  termP->newTerm.c_lflag &= ~(ISIG|ICANON|IEXTEN|ECHO|FLUSHO|PENDIN);
  termP->newTerm.c_lflag |= NOFLSH;

  if (cfsetspeed(&(termP->newTerm), selectedBaudRate)) {
  #ifdef DEBUG
    printf("setting modem serial device speed\n");
  #endif
  }

  if (tcsetattr(termP->fd, TCSADRAIN, &(termP->newTerm))) {
  #ifdef DEBUG
    printf("setting modem serial device attr\n");
  #endif
  }
}

int c_set_dtr(int fd, Bool val_bit) {
  int status;

  ioctl(fd, TIOCMGET, &status);
  if (val_bit)
    status |= TIOCM_DTR;
  else
    status &= ~TIOCM_DTR;

  ioctl(fd, TIOCMSET, &status);
  return 0;
}

/* From the gPhoto I/O library */
int c_serial_set_baudrate(int fd, const int speed) {
  struct termios tio;

  speed_t br = matchBaudRate(speed);

  if (tcgetattr(fd, &tio) < 0) {
    printf("tcgetattr\n");
  }

  tio.c_iflag = 0;
  tio.c_oflag = 0;
  tio.c_cflag = CS8 | CREAD | CLOCAL;
  tio.c_cc[VMIN] = 1;
  tio.c_cc[VTIME] = 5;

  tio.c_lflag &= ~(ICANON | ISIG | ECHO | ECHONL | ECHOE | ECHOK);

  cfsetispeed(&tio, br);
  cfsetospeed(&tio, br);
  if (tcsetattr(fd, TCSANOW | TCSAFLUSH, &tio) < 0) {
    printf("tcsetattr\n");
  }
  return 0;
}

/*
   $Id$
   Copyright (C) 2004 Pascal Brisset, Antoine Drouin
   Copyright (C) 2013 Stephen Dwyer

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

#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <sys/termios.h>
#include <sys/ioctl.h>

#include "../include/cserial.h"

// #include <caml/mlvalues.h>
// #include <caml/fail.h>
// #include <caml/alloc.h>

//static int baudrates[] = { B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400 };


/****************************************************************************/
/* Open serial device for requested protocoll */
/****************************************************************************/
int c_init_serial(char* device, int speed, Bool hw_flow_control)
{
  struct termios orig_termios, cur_termios;

  int br;

  switch (speed) {
    case 0:
      br = B0;
      break;
    case 50:
      br = B50;
      break;
    case 75:
      br = B75;
      break;
    case 110:
      br = B110;
      break;
    case 134:
      br = B134;
      break;
    case 150:
      br = B150;
      break;
    case 200:
      br = B200;
      break;
    case 300:
      br = B300;
      break;
    case 600:
      br = B600;
      break;
    case 1200:
      br = B1200;
      break;
    case 1800:
      br = B1800;
      break;
    case 2400:
      br = B2400;
      break;
    case 4800:
      br = B4800;
      break;
    case 9600:
      br = B9600;
      break;
    case 19200:
      br = B19200;
      break;
    case 38400:
      br = B38400;
      break;
    case 57600:
      br = B57600;
      break;
    case 115200:
      br = B115200;
      break;
    case 230400:
      br = B230400;
      break;
    default:
      printf("bad baudrate\n");
      break;
  }

  int fd = open(device, O_RDWR|O_NONBLOCK);

  if (fd == -1) printf("opening modem serial device : fd < 0\n");

  if (tcgetattr(fd, &orig_termios)) printf("getting modem serial device attr\n");
  cur_termios = orig_termios;

  /* input modes */
  cur_termios.c_iflag &= ~(IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK|ISTRIP|INLCR|IGNCR
			    |ICRNL |IXON|IXANY|IXOFF|IMAXBEL);
  /* pas IGNCR sinon il vire les 0x0D */
  cur_termios.c_iflag |= BRKINT;

  /* output_flags */
  cur_termios.c_oflag  &=~(OPOST|ONLCR|OCRNL|ONOCR|ONLRET);

  /* control modes */
  if (hw_flow_control) {
    cur_termios.c_cflag &= ~(CSIZE|CSTOPB|CREAD|PARENB|PARODD|HUPCL|CLOCAL);
    cur_termios.c_cflag |= CREAD|CS8|CLOCAL|CRTSCTS;
  }
  else {
    cur_termios.c_cflag &= ~(CSIZE|CSTOPB|CREAD|PARENB|PARODD|HUPCL|CLOCAL|CRTSCTS);
    cur_termios.c_cflag |= CREAD|CS8|CLOCAL;
  }

  /* local modes */
  cur_termios.c_lflag &= ~(ISIG|ICANON|IEXTEN|ECHO|FLUSHO|PENDIN);
  cur_termios.c_lflag |= NOFLSH;

  if (cfsetspeed(&cur_termios, br)) printf("setting modem serial device speed\n");

  if (tcsetattr(fd, TCSADRAIN, &cur_termios)) printf("setting modem serial device attr\n");

  return fd;
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
int c_serial_set_baudrate(int fd, int speed)
{
  struct termios tio;

  int br;

  switch (speed) {
    case 0:
      br = B0;
      break;
    case 50:
      br = B50;
      break;
    case 75:
      br = B75;
      break;
    case 110:
      br = B110;
      break;
    case 134:
      br = B134;
      break;
    case 150:
      br = B150;
      break;
    case 200:
      br = B200;
      break;
    case 300:
      br = B300;
      break;
    case 600:
      br = B600;
      break;
    case 1200:
      br = B1200;
      break;
    case 1800:
      br = B1800;
      break;
    case 2400:
      br = B2400;
      break;
    case 4800:
      br = B4800;
      break;
    case 9600:
      br = B9600;
      break;
    case 19200:
      br = B19200;
      break;
    case 38400:
      br = B38400;
      break;
    case 57600:
      br = B57600;
      break;
    case 115200:
      br = B115200;
      break;
    case 230400:
      br = B230400;
      break;
    default:
      printf("bad baudrate\n");
      break;
  }

  if (tcgetattr(fd, &tio) < 0) {
    printf("tcgetattr\n");
  }
  tio.c_iflag = 0;
  tio.c_oflag = 0;
  tio.c_cflag = CS8 | CREAD | CLOCAL;
  tio.c_cc[VMIN] = 1;
  tio.c_cc[VTIME] = 5;

  tio.c_lflag &= ~(ICANON | ISIG | ECHO | ECHONL | ECHOE | ECHOK);

  //int br = baudrates[Int_val(speed)];

  cfsetispeed(&tio, br);
  cfsetospeed(&tio, br);
  if (tcsetattr(fd, TCSANOW | TCSAFLUSH, &tio) < 0) {
    printf("tcsetattr\n");
  }
  return 0;
}
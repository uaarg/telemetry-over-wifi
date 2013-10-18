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

#ifndef _CSERIAL_H
#define _CSERIAL_H
  #include "dataTypes.h"

  int c_init_serial(
    const char* devicePath, const int speed, const Bool hw_flow_control
  );

  int c_set_dtr(int fd, const Bool val_bit);

  /* From the gPhoto I/O library */
  int c_serial_set_baudrate(int fd, const int speed);
#endif

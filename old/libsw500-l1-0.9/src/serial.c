/*******************************************************************************
* serial.c
* To Do:
* - */

/*******************************************************************************
* Copyright (C) 2005  Andrew Smith
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of version 2 of the GNU General Public License as published
* by the Free Software Foundation */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h> /* for opening port */
#include <termios.h>
#include <unistd.h>

#include "defines.h"
#include "misc.h"
#include "serial.h"

/* global, only used by initSerial() and unInitSerial() */
static struct termios oldtio; /* original serial port settings */

/*******************************************************************************
* initSerial()
* -opens the serial port and initializes it for use by the lbb library
*
* Recieves:
* - void
*
* Returns:
* - sd, open port
* - -1 if failed something
*
* Bugs:
* - */
int initSerial(void)
{
    /* vars for serial port control */
    int openPort;
    struct termios newtio; /* new serial port settings */
    
    /* Open port for reading and writing and not as controlling tty,
    * The O_NDELAY flag tells UNIX that this program doesn't care what state
    * the DCD signal line is in - whether the other end of the port is up and
    * running. If you do not specify this flag, your process will be put to
    * sleep until the DCD signal line is the space voltage */
    openPort = open(SER_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
    
    if(openPort < 0)
    {
        fprintf(stderr, "initSerial(): unable to open serial port\n");
        return(-1);
    }

    /* dont know what this is */
    fcntl(openPort, F_SETFL, 0);
    /* save current port settings */
    tcgetattr(openPort, &oldtio);

    /* MAKE NEW PORT SETTINGS */
    /* clear the newtio structure for new port settings */
    bzero(&newtio, sizeof(newtio));

    /* BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
    *  CRTSCTS : output hardware flow control (only used if the cable has
    *            all necessary lines)
    *  CS8     : 8n1 (8bit,no parity,1 stopbit)
    *  CLOCAL  : local connection, no modem contol
    *  CREAD   : enable receiving characters */
    /* control flag */
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    /* output flag: raw output */
    newtio.c_oflag = 0;
    /* inter-character timer */
    newtio.c_cc[VTIME] = 1;
    /* blocking read until x chars received */
    newtio.c_cc[VMIN] = 0;
    /* END MAKE NEW PORT SETTINGS */

    /* clean the line and activate settings for the port */
    tcflush(openPort, TCIFLUSH);
    tcsetattr(openPort, TCSANOW, &newtio);
    
    return openPort;
}

/*******************************************************************************
* unInitSerial()
* -restores serial port settings and closes the socket
*
* Recieves:
* - sd, open port
*
* Returns:
* - 1 if closed ok
* - -1 if sd recieved was invalid
* - -2 if failed to close
*
* Bugs:
* - */
int unInitSerial(int openPort)
{
    int closeReturn;
    
    if(openPort < 0)
    {
        fprintf(stderr, "unInitSerial(): called with sd=%d\n", openPort);
        return -1;
    }
    
    /* restore the old port settings */
    tcsetattr(openPort, TCSANOW, &oldtio);

    closeReturn = close(openPort);
    if(closeReturn < 0)
    {
        fprintf(stderr, "unInitSerial(): failed to close %d\n", openPort);
        return -2;
    }
    
    return 1;
}

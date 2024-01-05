/*******************************************************************************
* scontrol.c
* To Do:
* -  */

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
#include "scontrol.h"

/*******************************************************************************
* checkBuffState()
* -sends the 'Check Buffer State' command to the lbb and reads return
* -doesn't return the results from the lbb to the caller
*
* Recieves:
* - an open() type pointer to the serial port
* - pointer to unsigned short
* - pointer to unsigned char
*
* Returns:
* - 1 if all seems ok
* - -1 if error writing to port
* - -2 if error reading expected return
*  Via Parameters:
*  - data bytes (0 if error)
*  - logged data runs (0 if error)
*
* Bugs:
* - */
int checkBuffState(int openPort, unsigned short *dataBytes, unsigned char *loggedDataRuns)
{
    const char command = 0x26;
    
    int wroteCh; /* flag, wrote a byte */
    int numChRead; /* return from read() */
    
    wroteCh = writeP(openPort, (char*)&command, 1);
    if(wroteCh != 1)
    {
        fprintf(stderr, "checkBuffState(): unable to send command\n");
        return -1;
    }
    
    numChRead = readP(openPort, dataBytes, 2);
    if(numChRead != 2)
    {
        fprintf(stderr, "checkBuffState(): data bytes in buffer not read (%d)\n", numChRead);
        return -2;
    }

    numChRead = readP(openPort, loggedDataRuns, 1);
    if(numChRead != 1)
    {
        fprintf(stderr, "checkBuffState(): logged data runs in buffer not read (%d)\n", numChRead);
        return -2;
    }

    return 1;
}

/*******************************************************************************
* checkSampleState()
* -sends the 'Check Sample State' command to the lbb
*
* Recieves:
* - an open() type pointer to the serial port
*
* Returns:
* - 1 if all seems ok
* - -1 if error writing to port
*
* Bugs:
* - */
int checkSampleState(int openPort)
{
    const char command = 0x2A;
    
    int wroteCh;

    wroteCh = writeP(openPort, (char*)&command, 1);
    if(wroteCh != 1)
    {
        fprintf(stderr, "checkSampleState(): unable to send command\n");
        return -1;
    }

    /* now the caller should read the sample state record */
    
    return 1;
}

/*******************************************************************************
* disableDataLogging()
* -sends the 'Disable Data Logging' command to the lbb
*
* Recieves:
* - an open() type pointer to the serial port
*
* Returns:
* - 1 if all seems ok
* - -1 if error writing to port
*
* Bugs:
* - */
int disableDataLogging(int openPort)
{
    const char command = 0x2C;
    
    int wroteCh;

    wroteCh = writeP(openPort, (char*)&command, 1);
    if(wroteCh != 1)
    {
        fprintf(stderr, "disableDataLogging(): unable to send command\n");
        return -1;
    }
    
    return 1;
}

/*******************************************************************************
* enableDataLogging()
* -sends the 'Enable Data Logging' command to the lbb
*
* Recieves:
* - an open() type pointer to the serial port
*
* Returns:
* - 1 if all seems ok
* - -1 if error writing to port
*
* Bugs:
* - */
int enableDataLogging(int openPort)
{
    const char command = 0x2B;
    
    int wroteCh;

    wroteCh = writeP(openPort, (char*)&command, 1);
    if(wroteCh != 1)
    {
        fprintf(stderr, "enableDataLogging(): unable to send command\n");
        return -1;
    }
    
    return 1;
}

/*******************************************************************************
* oneShotSample()
* -sends the 'OneShotSample' command to the lbb
*
* Recieves:
* - an open() type pointer to the serial port
*
* Returns:
* - 1 if all seems ok
* - -1 if error writing to port
*
* Bugs:
* - */
int oneShotSample(int openPort)
{
    const char command = 0x27;
    
    int wroteCh;

    wroteCh = writeP(openPort, (char*)&command, 1);
    if(wroteCh != 1)
    {
        fprintf(stderr, "oneShotSample(): unable to send command\n");
        return -1;
    }
    
    /* now the caller should read the sample record */
    
    return 1;
}

/*******************************************************************************
* pauseSampling()
* -sends the 'Pause Sampling' command to the lbb
*
* Recieves:
* - an open() type pointer to the serial port
*
* Returns:
* - 1 if all seems ok
* - -1 if error writing to port
*
* Bugs:
* - */
int pauseSampling(int openPort)
{
    const char command = 0x23;
    
    int wroteCh;
    int returnCode;

    wroteCh = writeP(openPort, (char*)&command, 1);

    if(wroteCh != 1)
    {
        returnCode = -1;
        fprintf(stderr, "pauseSampling(): unable to send command\n");
    }
    else
    {
        returnCode = 1;
    }

    return returnCode;
}

/*******************************************************************************
* readBuff()
* -sends the 'Read Buffer' command to the lbb
*
* Recieves:
* - an open() type pointer to the serial port
*
* Returns:
* - 1 if all seems ok
* - -1 if error writing to port
*
* How it works:
* -sends command, no operands
* -recieves a sample data record
*
* Bugs:
* - */
int readBuff(int openPort)
{
    const char command = 0x29;
    
    int chWritten; /* flag, a byte was written */
    
    /* send the command */
    chWritten = writeP(openPort, (char*)&command, 1);
    if(chWritten != 1)
    {
        fprintf(stderr, "readBuffer(): unable to send command (%d)\n", chWritten);
        return -1;
    }
    
    /* now get all the records the lbb will return */
    /* (in a different function) */
    
    return 1;
}

/*******************************************************************************
* resetBuff()
* -sends the 'Reset Buffer' command to the lbb
*
* Recieves:
* - an open() type pointer to the serial port
*
* Returns:
* - 1 if all seems ok
* - -1 if error writing to port
*
* Bugs:
* - */
int resetBuff(int openPort)
{
    const char command = 0x25;
    
    int wroteCh;
    int returnCode;

    wroteCh = writeP(openPort, (char*)&command, 1);

    if(wroteCh != 1)
    {
        returnCode = -1;
        fprintf(stderr, "resetBuff(): unable to send command\n");
    }
    else
    {
        returnCode = 1;
    }

    return returnCode;
}

/*******************************************************************************
* resumeSampling()
* -sends the 'Resume Sampling' command to the lbb
*
* Recieves:
* - an open() type pointer to the serial port
*
* Returns:
* - 1 if all seems ok
* - -1 if error writing to port
*
* Bugs:
* - */
int resumeSampling(int openPort)
{
    const char command = 0x24;
    
    int wroteCh;
    int returnCode;

    wroteCh = writeP(openPort, (char*)&command, 1);

    if(wroteCh != 1)
    {
        returnCode = -1;
        fprintf(stderr, "resumeSampling(): unable to send command\n");
    }
    else
    {
        returnCode = 1;
    }

    return returnCode;
}

/*******************************************************************************
* startSampling()
* -sends the 'Start Sampling' command to the lbb
*
* Recieves:
* - an open() type pointer to the serial port
*
* Returns:
* - 1 if all seems ok
* - -1 if error writing to port
*
* How it works:
* -sends command, no operands
* -recieves a sample data record
*
* Bugs:
* - */
int startSampling(int openPort)
{
    const char command = 0x21;
    
    int chWritten; /* flag, a byte was written */
    
    /* send the command */
    chWritten = writeP(openPort, (char*)&command, 1);
    if(chWritten != 1)
    {
        fprintf(stderr, "startSampling(): unable to send command (%d)\n", chWritten);
        return -1;
    }
    
    /* now keep trying to get a record */
    /* (in a different function) */
    
    return 1;
}

/*******************************************************************************
* stopSampling()
* -sends the 'Stop Sampling' command to the lbb
*
* Recieves:
* - an open() type pointer to the serial port
*
* Returns:
* - 1 if all seems ok
* - -1 if error writing to port
*
* Bugs:
* -the command sent is echoed to stdout in ascii - fixed (function call
*  had no parameters) */
int stopSampling(int openPort)
{
    const char command = 0x22;
    
    int wroteCh;
    int returnCode;

    wroteCh = writeP(openPort, (char*)&command, 1);

    if(wroteCh != 1)
    {
        returnCode = -1;
        fprintf(stderr, "stopSampling(): unable to send command\n");
    }
    else
    {
        returnCode = 1;
    }

    return returnCode;
}

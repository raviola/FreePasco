/*******************************************************************************
* readrec.c
* functions to read records from the lbb
* To Do:
* - 
* Bugs:
* - potential: see readRecordCS() */

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
#include "readrec.h"

/*******************************************************************************
* readRecordCS()
* -reads a 'clocked sample record' (0x1d)
*
* Recieves:
* - open() type pointer to the serial port
* - array of 10 bytes (same as inputSelect()) to tell which channels selected
* - array of 3 short ints for analog readings (selected or not)
* - array of 2 unsigned short ints for digital readings (selected or not)
*
* Returns:
* - 1 if all seems ok
* - -1 if error reading expected return
*  Via Parameters:
*  - reading for each analog channel
*  - reading for each digital channel
*  * arrays zeroed in case of error
*  * elements of arrays set to zero if particular channel not selected
*
* Bugs:
* - 
* Notes:
* - understand what the 'd' int 0x1d is */
int readRecordCS(int openPort, char channels[10], short analog[3], unsigned short digital[2])
{
    int numChRead;
    
    short junk;

    if(channels[0])
    /* analog channel A (no gain) */
    {
        /* read first byte */
        numChRead = readP(openPort, &(analog[0]), 2);
        if(numChRead != 2)
        {
            fprintf(stderr, "readRecordCS(): channel A (no gain) read %d"
                            " instead of 2 bytes\n", numChRead);
            bzero(analog, 6);
            bzero(digital, 4);
            return -1;
        }
    }
    else
    {
        /* might be reading the other channel A
        * ( *analog only has one value for both channels[0] and channels[1] */
        if( !channels[1] )
            analog[0] = -1;
    }
    
    if(channels[1])
    /* analog channel A (with gain)
    * go figure, this one gets 2 bytes that are an exact copy of the first 2 */
    {
        numChRead = readP(openPort, &(analog[0]), 2);
        if(numChRead != 2)
        {
            fprintf(stderr, "readRecordCS(): channel A (with gain) read %d"
                            " instead of 2 bytes\n", numChRead);
            bzero(analog, 6);
            bzero(digital, 4);
            return -1;
        }
        
        /* for some reason i get 2 bytes of nothing when reading with gain */
        /* 15 feb 05: not today !? */
        //readP(openPort, &junk, 2);
    }
    else
    {
        /* might be reading the other channel A */
        if( !channels[0] )
            analog[0] = -2;
    }
    
    if(channels[2])
    /* analog channel B (no gain) */
    {
        numChRead = readP(openPort, &(analog[1]), 2);
        if(numChRead != 2)
        {
            fprintf(stderr, "readRecordCS(): channel B (no gain) read %d"
                            " instead of 2 bytes\n", numChRead);
            bzero(analog, 6);
            bzero(digital, 4);
            return -1;
        }
    }
    else
    {
        /* might be reading the other channel A */
        if( !channels[3] )
            analog[1] = -3;
    }

    if(channels[3])
    /* analog channel B (with gain)
    * go figure, this one gets 2 bytes that are an exact copy of the first 2 */
    {
        numChRead = readP(openPort, &(analog[1]), 2);
        if(numChRead != 2)
        {
            fprintf(stderr, "readRecordCS(): channel B (with gain) read %d"
                            " instead of 2 bytes\n", numChRead);
            bzero(analog, 6);
            bzero(digital, 4);
            return -1;
        }

        readP(openPort, &junk, 2);
    }
    else
    {
        /* might be reading the other channel A */
        if( !channels[2] )
            analog[1] = -4;
    }
   
    if(channels[4])
    /* analog channel C */
    {
        numChRead = readP(openPort, &(analog[2]), 2);
        if(numChRead != 2)
        {
            fprintf(stderr, "readRecordCS(): channel C read %d"
                            " instead of 2 bytes\n", numChRead);
            bzero(analog, 6);
            bzero(digital, 4);
            return -1;
        }
    }
    else
        analog[2] = -5;

    if(channels[5])
    /* digital channel 1 */
    {
        numChRead = readP(openPort, &(digital[0]), 2);
        if(numChRead != 2)
        {
            fprintf(stderr, "readRecordCS(): digital channel 1 read %d"
                            " instead of 2 bytes\n", numChRead);
            bzero(analog, 6);
            bzero(digital, 4);
            return -1;
        }
    }
    else
        digital[0] = -1;
    
    if(channels[6])
    /* digital channel 2 */
    {
        numChRead = readP(openPort, &(digital[1]), 2);
        if(numChRead != 2)
        {
            fprintf(stderr, "readRecordCS(): digital channel 1 read %d"
                            " instead of 2 bytes\n", numChRead);
            bzero(analog, 6);
            bzero(digital, 4);
            return -1;
        }
    }
    else
        digital[1] = -1;

    return 1;
}

/*******************************************************************************
* readRecordDE()
* -reads a 'digital event record' (0x2d)
*
* Recieves:
* - open() type pointer to the serial port
* - pointer to an unsigned int
*
* Returns:
* - 1 if all seems ok
* - -1 if error reading expected return
*  Via Parameters:
*  - time of event
*    0 if error
*
* Bugs:
* - understand what the 'd' int 0x2d is
* Notes:
* -  */
int readRecordDE(int openPort, unsigned* timeOfEvent)
{    
    int numChRead;
    
    numChRead = readP(openPort, timeOfEvent, 4);
    if(numChRead != 4)
    {
        fprintf(stderr, "readRecordDE(): time of event not read (%d)\n", numChRead);
        *timeOfEvent = 0;
        return -1;
    }

    return 1;
}

/*******************************************************************************
* readRecordMT()
* -reads a 'motion timer record' (0x50)
*
* Recieves:
* - open() type pointer to the serial port
* - pointer to an unsigned short int
* - pointer to an unsigned int
*
* Returns:
* - 1 if all seems ok
* - -1 if error reading expected return
*  Via Parameters:
*  - timerValue (0 if error)
*  - timeOfEcho (0 if error)
*
* Bugs:
* - 
* Notes:
* -  */
int readRecordMT(int openPort, unsigned short *timerValue, unsigned *timeOfEcho)
{
    int numChRead;
    
    numChRead = readP(openPort, timerValue, 2);
    if(numChRead != 2)
    {
        fprintf(stderr, "readRecordMT(): timer value not read (%d)\n", numChRead);
        *timerValue = 0;
        *timeOfEcho = 0;
        return -1;
    }
        
    numChRead = readP(openPort, timeOfEcho, 4);
    if(numChRead != 4)
    {
        fprintf(stderr, "readRecordMT(): time of event not read (%d)\n", numChRead);
        *timerValue = 0;
        *timeOfEcho = 0;
        return -1;
    }

    return 1;
}

/*******************************************************************************
* readRecordTTO()
* -reads a 'time stamp offset record' (0xF0)
*
* Recieves:
* - open() type pointer to the serial port
* - pointer to an unsigned int
*
* Returns:
* - 1 if all seems ok
* - -1 if error reading expected return
*  Via Parameters:
*  - timerStampOffset (0 if error)
*
* Bugs:
* - 
* Notes:
* -  */
int readRecordTTO(int openPort, unsigned *timeStampOffset)
{
    int numChRead;
    
    numChRead = readP(openPort, &timeStampOffset, 4);
    if(numChRead != 4)
    {
        fprintf(stderr, "readRecordTTO(): time stamp offset not read (%d)\n", numChRead);
        *timeStampOffset = 0;
        return -1;
    }
    
    return 1;
}

/*******************************************************************************
* readRecordType()
* -reads a byte, type of record to follow
*
* Recieves:
* - open() type pointer to the serial port
*
* Returns:
* - -1 if error reading
* - -2 if read bad record type
* - 1 if clocked sample record
* - 2 if digital event record
* - 3 if pause record
* - 4 if motion timer record
* - 5 if sample state record
* - 6 if trigger time offset record
*
* Bugs:
* - 
* Notes:
* - understand what the 'd' int 0x2d is */
int readRecordType(int openPort)
{
    char recordType;
    int returnCode;
    int numChRead;
    
    /* first byte read is type of record to follow */
    numChRead = readP(openPort, &recordType, 1);
    if(numChRead != 1)
    {
        fprintf(stderr, "readRecordType(): no record type read\n");
        return -1;
    }

    /* SET the return code depending on the record type */
    
    if( getBit(4, recordType) && !getBit(5, recordType) &&
        !getBit(6, recordType) && !getBit(7, recordType) )
    /* clocked sample record */
    {
        returnCode = 1;
    }
    else if( !getBit(4, recordType) && getBit(5, recordType) &&
        !getBit(6, recordType) && !getBit(7, recordType) )
    /* digital event record */
    {
        returnCode = 2;
    }
    else if( !getBit(4, recordType) && !getBit(5, recordType) &&
             getBit(6, recordType) && !getBit(7, recordType) )
    /* pause record (0x40) */
    {
        returnCode = 3;
    }
    else if( getBit(4, recordType) && !getBit(5, recordType) &&
             getBit(6, recordType) && !getBit(7, recordType) )
    /* motion timer record (0x50) */
    {
        returnCode = 4;
    }
    else if( !getBit(4, recordType) && getBit(5, recordType) &&
             getBit(6, recordType) && !getBit(7, recordType) )
    /* sample state record (0x6s) */
    {
        returnCode = 5;
    }
    else if( getBit(4, recordType) && getBit(5, recordType) &&
             getBit(6, recordType) && getBit(7, recordType) )
    /* trigger time offset record (0xF0) */
    {
        returnCode = 6;
    }
    else
    {
        fprintf(stderr, "readRecordType(): bad record type read: ");fflush(stderr);
        printByte(recordType);
        putchar('\n');
        return -2;
    }
    
    /* END SET the return code depending on the record type */
    
    return returnCode;
}

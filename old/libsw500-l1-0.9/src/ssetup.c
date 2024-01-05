/*******************************************************************************
* ssetup.c
* To Do:
* -rotMotRes() parameter unknown type
* -logStateStore() and logStateRetrieve() do nothing what should they do? */

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
#include "ssetup.h"

/*******************************************************************************
* inputSelect()
* -sends command to select the input channel(s)
*
* How it works:
* -sends command followed by 2 operand bytes (no more then one channel
*  can be selected, hex values are for whole byte):
*  -byte1:
*   bit0 (0x01) - analog ch A (a)
*   bit1 (0x02) - analog ch A with gain 10X (A)
*   bit2 (0x04) - analog ch B (b)
*   bit3 (0x08) - analog ch B with gain 10X (B)
*   bit4 (0x10) - analog ch C (c)
*   bit5-7 - unused
*  -byte2:
*   bit0 - unused
*   bit1 - unused
*   bit2 (0x04) - digital ch 1 events
*   bit3 (0x08) - digital ch 2 events
*   bit4 (0x10) - digital ch 1 counts
*   bit5 (0x20) - digital ch 2 counts
*   bit6 - unused
*   bit7 (0x80) - motion timer flag
*
* Recieves:
* - an open() type pointer to the serial port
* - char* list of channels to select, 
*   10-byte array each element true or false:
*   channels[0] for analog channel A
*   channels[1] for analog channel A with gain 10x
*   channels[2] for analog channel B
*   channels[3] for analog channel B with gain 10x
*   channels[4] for analog channel C
*   channels[5] for digital channel 1 events
*   channels[6] for digital channel 2 events
*   channels[7] for digital channel 1 counts
*   channels[8] for digital channel 2 counts
*   channels[9] for motion timer flag
*
* Returns:
* - 1 if successfull
* - 0 if not
*
* To do:
* -
*
* Bugs:
* - */
int inputSelect(int openPort, char channels[10])
{
    const char command = 0x11;
    
    char opByte1, opByte2; /* operand bytes */
    int wroteCh; /* return from write() */
    
    opByte1 = 0;
    opByte2 = 0;
    
    /* following chain of ifs sets the operand bytes according to the 
    * instructions recieved */
    if(channels[0])
        opByte1 |= 0x01;
    if(channels[1])
        opByte1 |= 0x02;
    if(channels[2])
        opByte1 |= 0x04;
    if(channels[3])
        opByte1 |= 0x08;
    if(channels[4])
        opByte1 |= 0x10;

    if(channels[5])
        opByte2 |= 0x04;
    if(channels[6])
        opByte2 |= 0x08;
    if(channels[7])
        opByte2 |= 0x10;
    if(channels[8])
        opByte2 |= 0x20;
    if(channels[9])
        opByte2 |= 0x80;

    /* now send the command followed by the 2 opbytes */
    
    wroteCh = writeP(openPort, (char*)&command, 1);
    if(wroteCh != 1)
    {
        fprintf(stderr, "inputSelect(): couldn't send a byte (command)\n");
        return 0;
    }
    
    wroteCh = writeP(openPort, &opByte1, 1);
    if(wroteCh != 1)
    {
        fprintf(stderr, "identify(): couldn't send a byte (op byte 1)\n");
        return 0;
    }
    
    wroteCh = writeP(openPort, &opByte2, 1);
    if(wroteCh != 1)
    {
        fprintf(stderr, "identify(): couldn't send a byte (op byte 2)\n");
        return 0;
    }
    
    return 1;
}

/*******************************************************************************
* logStateStore()
* -sends command to "store log mode state data from the host in a specific 
*  ram area for later retrieval"
* -max number of bytes: 3072
*
* How it works:
* -sends command followed by 1 operand (numBytes)
* -then probably sends a stream of bytes to be stored
* -expects a checksum to be returned by the lbb:
*  *least significant byte
*  *of the one's cmpliment
*  *of the sum(signed short) of all the data bytes
*
* Recieves:
* - an open() type pointer to the serial port
* - unsigned short, number of bytes
* - char*, array of bytes to send
*
* Returns:
* - 1, all writes successfull
* - 0, a write failed */
int logStateStore(int openPort, unsigned short numBytes, char *bytes)
{
    /* don't know what the hell this functions is supposed to do */
    
    return -1;
}

/*******************************************************************************
* logStateRetrieve()
* -sends command to "retrieve log mode state data from a specific ram area"
* -max number of bytes: 3072
*
* How it works:
* -sends command
* -recieves the number of bytes to follow and then the bytes themselves
*
* Recieves:
* - an open() type pointer to the serial port
* - unsigned short *
* - char *
*
* Returns:
* - 1, all writes successfull
* - 0, a write failed
* Via Parameters:
*  -number of bytes (size of array)
*  -array of bytes recieved */
int logStateRetrieve(int openPort, unsigned short *numBytes, char *bytes)
{
    /* don't know what the hell this functions is supposed to do */
    
    return -1;
}

/*******************************************************************************
* rateSelect()
* -sends command to select the sampling rate
*
* How it works:
* -sends command followed by 4 operands:
*  -op1: 4 bytes (unsigned long)
*   sample period
*  -op2: 4 bytes (unsigned long)
*   clock period
*  -op3: 2 bytes (unsigned short)
*   ping period
*  -op4: 1 byte
*   small buffer flag
*
* Recieves:
* - an open() type pointer to the serial port
* - unsigned long, sample period (microseconds)
* - unsigned long, clock period (sample periods)
* - unsigned short, ping period (100 microsecond ticks)
* - char, small buffer flag (1=small, 0=large)
*
* Returns:
* - 1, all writes successfull
* - 0, a write failed */
int rateSelect(int openPort, unsigned long samplePeriodIn, 
               unsigned long clockPeriodIn, unsigned short pingPeriodIn, 
               char smallBufferIn)
{
    const char command = 0x12;

    /* the 4 operands to send to the lbb */
    unsigned long samplePeriod;
    unsigned long clockPeriod;
    unsigned short pingPeriod;
    char smallBuffer;

    int numChWritten;

    /* 50 microseconds */
    samplePeriod = samplePeriodIn;
    /* no digital */
    clockPeriod = clockPeriodIn;
    /* unknown value, try 1 */
    pingPeriod = pingPeriodIn;
    /* large buffer */
    smallBuffer = smallBufferIn;

    /* send command */
    numChWritten = writeP(openPort, (char*)&command, 1);
    if(numChWritten != 1)
    {
        fprintf(stderr, "rateSelect(): unable to send command\n");
        return 0;
    }
    
    /* send operand 1 */
    numChWritten = writeP(openPort, &samplePeriod, 4);
    if(numChWritten != 4)
    {
        fprintf(stderr, "rateSelect(): unable to send operand 1 (%d)\n", numChWritten);
        return 0;
    }
        
    /* send operand 2 */
    numChWritten = writeP(openPort, &clockPeriod, 4);
    if(numChWritten != 4)
    {
        fprintf(stderr, "rateSelect(): unable to send operand 2 (%d)\n", numChWritten);
        return 0;
    }
            
    /* send operand 3 */
    numChWritten = writeP(openPort, &pingPeriod, 2);
    if(numChWritten != 2)
    {
        fprintf(stderr, "rateSelect(): unable to send operand 3 (%d)\n", numChWritten);
        return 0;
    }
                
    /* send operand 4 */
    numChWritten = writeP(openPort, &smallBuffer, 1);
    if(numChWritten != 1)
    {
        fprintf(stderr, "rateSelect(): unable to send operand 4 (%d)\n", numChWritten);
        return 0;
    }

    return 1;
}

/*******************************************************************************
* rotaryMotRes()
* -sends the 'Rotary Motion Resolution' command
*
* Recieves:
* - an open() type pointer to the serial port
* - char, 1 or 2
*
* Returns:
* - 1 if everything ok
* - 0 if communication falure
* */
int rotaryMotRes(int openPort, char numPulses)
{
    const char command = 0x14;
    
    int wroteCh;

    wroteCh = writeP(openPort, (char*)&command, 1);
    if(wroteCh != 1)
    {
        fprintf(stderr, "rotaryMotRes(): unable to send command\n");
        return 0;
    }
    
    wroteCh = writeP(openPort, &numPulses, 1);
    if(wroteCh != 1)
    {
        fprintf(stderr, "rotaryMotRes(): unable to send number of pulses\n");
        return 0;
    }

    return 1;
}

/*******************************************************************************
* triggerSelect()
* -sends the 'Trigger Select' command
*
* How it works:
* -sends command followed by 3 operands:
*  -op1 (unsigned byte), channel (read trigger on ..):
*   1 - first selected analog channel
*   2 - second selected analog channel
*   3 - third selected analog channel
*   4 - digital channel 1
*   5 - digital channel 2
*  -op2 (unsigned byte), slope (read trigger on ..):
*   0 - falling analog signal or low digital state
*   1 - rising analog signal or high digital state
*  -op3 (signed short, 2 bytes), level:
*   don't understand this one, (10*32767)/10 seems to make sense
*
* Recieves:
* - an open() type pointer to the serial port
* - unsigned char, channel
* - int, slope
* - int, level
*
* Returns:
* - 1 if everything ok
* - 0 if communication falure
* */
int triggerSelect(int openPort, unsigned char channel, unsigned char slope, 
                                                            signed short level)
{
    const char command = 0x13;
    int wroteCh; /* return from write() */
    
    /* send command */
    wroteCh = writeP(openPort, (char*)&command, 1);
    if(wroteCh != 1)
    {
        fprintf(stderr, "triggerSelect(): couldn't send a byte (command)\n");
        return 0;
    }

    /* send op byte 1 (channel) */
    wroteCh = writeP(openPort, &channel, 1);
    if(wroteCh != 1)
    {
        fprintf(stderr, "triggerSelect(): couldn't send a byte (op 1)\n");
        return 0;
    }
    
    /* send op byte 2 (slope) */
    wroteCh = writeP(openPort, &slope, 1);
    if(wroteCh != 1)
    {
        fprintf(stderr, "triggerSelect(): couldn't send a byte (op 2)\n");
        return 0;
    }
    
    /* send op byte 3 (level) */
    wroteCh = writeP(openPort, &level, 2);
    if(wroteCh != 2)
    {
        fprintf(stderr, "triggerSelect(): couldn't send a byte (op 3)\n");
        return 0;
    }

    return 1;
}

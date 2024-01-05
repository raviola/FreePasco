/*******************************************************************************
* init.c
* Initialization Commands:
* -Identify     (0x01)
* -Download     (0x02)
* -Execute      (0x03)
* -Reset        (0x04)
*
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
#include "init.h"

/*******************************************************************************
* execute()
* -sends the 'Execute' command to the lbb
*
* Recieves:
* - an open() type pointer to the serial port
*
* Returns:
* - -2 if return code wrong
* - -1 if comm error
* - 0 if lbb returned nothing (powered off or no code to execute)
* - 1 if lbb returned proper acknoledgement */
int execute(int openPort)
{
    const char command = 0x03;
    
    char ackn[MAX_REC_EX]; /* acknoledgement lbb sends, no null byte stuff */
    int returnCode;
    int count;
    int readCh, wroteCh; /* return from read/write() */

    bzero(ackn, MAX_REC_EX);

    /* send Execute command */
    wroteCh = writeP(openPort, (char*)&command, 1);
    if(wroteCh != 1)
    {
        fprintf(stderr, "execute(): failed to write command\n");
        fflush(stderr);
        return -1;
    }

    /* read return from the lbb, one byte at a time */
    count = 0;
    while (count < MAX_REC_EX)
    {
        readCh = readP(openPort, &ackn[count], 1);
        if(readCh != 1)
        {
            fprintf(stderr, "execute(): couldn't read a byte (part of acknoledgement)\n");
            fflush(stderr);
            return -1;
        }

        count++;
    }
    
    if( strncmp(ackn, "RAM code is running.", MAX_REC_EX) == 0 )
    {
        returnCode = 1;
    }
    else
    {
        fprintf(stderr, "execute(): bad return recieved\n");
        fflush(stderr);
        returnCode = -2;
    }

    return returnCode;
}

/*******************************************************************************
* identify()
* -sends the 'Identify' command to the lbb and reads the return
*
* Recieves:
* - an open() type pointer to the serial port
*
* Returns:
* - -1 if the return was wrong
* - 0 if communication error
* - 1 if in ROM mode
* - 2 if in RAM mode
*
* Notes:
* -'returnCode' is used to check there were no previous errors
*
* To do:
* -
*
* Bugs:
*  -sometimes the lbb does not return the version code...
*   apparently this is fixed now that input is read one byte at a time */
int identify(int openPort)
{
    const char command = 0x01; /* for writing to the lbb */

    char fromPascoCh; /* for reading from the lbb */
    char fromPascoAr[MAX_REC_ID]; /* for total read from the lbb */
    int readCh, wroteCh; /* flags - read, wrote a byte */
    int numChRead; /* number of bytes read, written */
    int numChFailed; /* number of bytes failed to read */
    int count;
    int returnCode;

    /* send 'Identify' command */
    wroteCh = writeP(openPort, (char*)&command, 1);
    if(wroteCh != 1)
    {
        fprintf(stderr, "identify(): couldn't send a byte (command)\n");
        fflush(stderr);
        return 0;
    }
    
    /* get return from 'Identify' by read()ing one char at a time */
    numChRead = 0;
    numChFailed = 0;
    bzero(fromPascoAr, MAX_REC_ID);
    for(count = 0; count < MAX_REC_ID && returnCode; count++)
    {
        readCh = readP(openPort, &fromPascoCh, 1);
        
        if(readCh == 1)
        {
            numChRead++;
            fromPascoAr[count] = fromPascoCh;
        }
        else if(readCh == 0)
        {
            numChFailed++;
        }
        else
        {
            /* read() might return -1 */
            fprintf(stderr, "identify(): couldn't read byte %d of reply\n", count);
            fflush(stderr);
            
            return 0;
        }
    }

    /* expecting to have recieveved one of the two:
    *  -"SW500i  1.0 " (12 bytes)
    *  -"SW500i  1.0 RAM" (15 bytes)
    * -those white spaces above are 0x20 (ascii SPACE)
    * -no null-byte or any other kind of termination expected */
    if(numChFailed > 3)
    {
        fprintf(stderr, "identify(): was unable to read %d bytes (that's bad)\n",
                numChFailed);
        fflush(stderr);
        return -1;
    }
    else if(numChFailed == 3)
    {
        if( strncmp(fromPascoAr, "SW500i  1.0 ", 12) == 0 )
        {
            returnCode = 1;
        }
        else
        {
            /* return code was wrong */
            fprintf(stderr, "identify(): recieved 12 bytes (good) but wrong content\n");
            fflush(stderr);
            return -1;
        }
    }
    else if(numChFailed == 0)
    {
        if( strncmp(fromPascoAr, "SW500i  1.0 RAM", 15) == 0 )
        {
            returnCode = 2;
        }
        else
        {
            /* return code was wrong */
            fprintf(stderr, "identify(): recieved 15 bytes (good) but wrong content\n");
            fflush(stderr);
            return -1;
        }
    }

    return returnCode;
}

/*******************************************************************************
* initialize()
* -a.k.a. download record
* -sends x 'Download' commands to the lbb and reads the return
*
* Notes:
* -this function is complicated, look for flow chart
* -file io not checked (except open()), contents assumed good
* -file must have at least one non-S2 line at the end (it will be ignored, 
*  only need it to know there is nothing else to read)
*
* Recieves:
* - an open() type pointer to the serial port
*
* Returns:
* - 1, success
* - 0, communication error
* - -1, file open error, line size error
* - -2, checksum error
*
* To do:
* - */
int initialize(int openPort)
{
    const char command = 0x02;

    int sendNextRecord;
    
    /* FILE IO */
    FILE *openFile = NULL;
    /* first and second characters on a line */
    char firstCh, secondCh;
    /* to use sscanf for char-->hex */
    char twoChArr[3];
    /* number of bytes in current record */
    int bytesThisLineInt; /* anti scanf("%X") warning */
    char bytesThisLine;
    /* byte (not char) line to be sent */
    int lineIntTmp; /* anti scanf("%X") warning */
    char line[MAX_S28_LINE_LEN];
    /* for reading a line */
    int count1;
    /* last byte of a record - checksum */
    char checksumSent;
    /* to hold what's supposed to be a \n */
    char newLine;
    
    /* COMM */
    /* read()/write() return */
    int readCh, wroteCh;
    /* for sending record */
    int count2;
    /* return from the lbb */
    char checksumReturned;
    /* number of times got wrong checksum */
    int numTimesWrong;
    
    openFile = fopen(PASCOFILE, "r");
    if(openFile == NULL)
    {
        fprintf(stderr, "initialize(): unable to open %s for reading\n", PASCOFILE);
        fflush(stderr);
        return -1;
    }
    
    numTimesWrong = 0;
    sendNextRecord = 1;

    /* send records */
    do
    {
        /* read first 2 characters on a line */
        firstCh = fgetc(openFile);
        secondCh = fgetc(openFile);
        
        /* is this line an S2 record or not */
        if(firstCh == 'S' && secondCh == '2')
        {
            /* the next byte has the number of characters in record */
            twoChArr[0] = fgetc(openFile);
            twoChArr[1] = fgetc(openFile);
            twoChArr[2] = '\n';
            sscanf(&twoChArr[0], "%X", &bytesThisLineInt);
            bytesThisLine = bytesThisLineInt;
            
            /* the byte i just read needs to be sent too */
            line[0] = bytesThisLine;
            
            /* read the record */
            for(count1 = 1; count1 <= bytesThisLine; count1++)
            {
                twoChArr[0] = fgetc(openFile);
                twoChArr[1] = fgetc(openFile);
                twoChArr[2] = '\n';
                sscanf(&twoChArr[0], "%X", &lineIntTmp);
                line[count1] = lineIntTmp;
            }
            
            /* copy last byte to a variable for later */
            checksumSent = line[count1 - 1];
            
            /* get the \n at the end */
            newLine = fgetc(openFile);
            if(newLine != '\n')
            {
                fprintf(stderr, "initialize(): %s corrupt (expected \'\\n\', got \'%c\')\n", PASCOFILE, newLine);
                fflush(stderr);
                
                return -1;
            }
            
            /* this loop is for resending a record in case of error */
            do
            {
                /* send download command */
                wroteCh = writeP(openPort, (char*)&command, 1);
                if(wroteCh != 1)
                {
                    fprintf(stderr, "identify(): couldn't send a byte (command)\n");
                    fflush(stderr);
                    
                    return 0;
                }
                
                /* send record */
                for(count2 = 0; count2 <= bytesThisLine; count2++)
                {
                    wroteCh = writeP(openPort, &line[count2], 1);
                    if(wroteCh != 1)
                    {
                        fprintf(stderr, "initialize(): couldn't send a byte (record piece)\n");
                        fflush(stderr);
                        return 0;
                    }
                }
                
                /* get return (gotta be the checksum of what i sent) */
                readCh = readP(openPort, &checksumReturned, 1);
                if(readCh != 1)
                {
                    fprintf(stderr, "initialize(): couldn't read a byte (checksum)\n");
                    fflush(stderr);
                    return 0;
                }
                
                /* check the checksum is right */
                if(checksumSent != checksumReturned)
                    numTimesWrong++;
                else
                    numTimesWrong = 0;
                
            } while (numTimesWrong > 0 && numTimesWrong <= MAX_WRONG_CHECKSUM);
            
            if(numTimesWrong > MAX_WRONG_CHECKSUM)
            {
                fprintf(stderr, "initialize(): got wrong checksum %d times\n", numTimesWrong + 1);
                fflush(stderr);
                return -2;
            }

        }
        else
        {
            /* not an S2 record, stop */
            sendNextRecord = 0;
        }
        
    } while (sendNextRecord);
    
    fclose(openFile);
    
    /* all errors have their own returns, this one good for sure */
    return 1;
}

/*******************************************************************************
* reset()
* -works in ram mode to reset the lbb to rom mode
*
* Recieves:
* - an open() type pointer to the serial port
*
* Returns:
* - 0 if failed to send command
* - 1 otherwise
*
* To do:
* -
*
* Bugs:
* -after sending the reset command, the lbb waits a second or two before
*  replying to any other command
*  -put a sleep() at the end to fix this */
int reset(int openPort)
{
    const char command = 0x04;
    int wroteCh = 0;
    
    wroteCh = writeP(openPort, (char*)&command, 1);

    if(wroteCh != 1)
    {
        fprintf(stderr, "reset(): couldn't send command\n");
        fflush(stderr);
        
        return 0;
    }

    usleep(RESET_SLEEP);
    
    return 1;
}

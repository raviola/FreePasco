/*******************************************************************************
* misc.c 
* all kinds of helper functions */

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

#include "misc.h"

/*******************************************************************************
* getBit()
* returns a certain bit from a given byte */
char getBit(int bit, char byte)
{
    /* if byte is 0010 0000 and bit is 5:
    * 1 << bit is 0010 0000
    * byte & (1 << bit) is 0010 0000
    * return is 0000 0001
    *
    *  if byte is 0010 0000 and bit is 4:
    * 1 << bit is 0001 0000
    * byte & (1 << bit) is 0000 0000
    * return is 0000 0000 */
    
    return (byte & (1 << bit)) >> bit;
}

/*******************************************************************************
* printByte()
* -displays a byte in the format "XXXX XXXX"
*
* Recieves:
* - char, byte to display
*
* Returns:
* - void */
void printByte(char byte)
{
    int count = 8;
    while(count--)
    {
        fprintf(stderr, "%d", ( byte & 128 ) ? 1 : 0 );
        if(count == 4)
            putchar(' ');
        byte <<= 1;
    }
}

// TODO: check for -1 from read/write
int readP(int fd, void* data, unsigned numBytes)
{
    int count;
    int returnCode = 0;
    
    /* pasco has bytes reversed compared to intel machines,
    * so first byte read is last */
    for(count = numBytes - 1; count >= 0; count--)
    {
        /* need the address of a byte */
        returnCode += read(fd, (char*)data + count, 1);
    }
    
    return returnCode;
}

int writeP(int fd, void* data, unsigned numBytes)
{
    int count;
    int returnCode = 0;
    
    /* pasco has bytes reversed compared to intel machines,
    * so first byte to be written is last */
    for(count = numBytes - 1; count >= 0; count--)
    {
        /* need the address of a byte */
        returnCode += write(fd, (char*)data + count, 1);
    }
    
    return returnCode;
}

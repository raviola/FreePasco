#ifndef misc_h
#define misc_h

/* FUNCTION prototypes: */
char getBit(int bit, char byte);
void printByte(char byte);
int readP(int fd, void* data, unsigned numBytes);
int writeP(int fd, void* data, unsigned numBytes);
/* END FUNCTION prototypes: */

#endif /* #ifndef misc_h */

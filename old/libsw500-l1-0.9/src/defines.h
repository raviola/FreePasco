/* SER_PORT is defined in the makefile */
#define BAUDRATE B19200

/* PASCOFILE is defined in the makefile */
/* SER_PORT is defined in the makefile */

/* max length of a line in sw500ram.s28, this is rather loose */
#define MAX_S28_LINE_LEN 50

/* max size of return recieved by identify() */
#define MAX_REC_ID 15
/* max size of return recieved by execute() */
#define MAX_REC_EX 20
/* max times getting wrong checksum is ok (initialize()) */
#define MAX_WRONG_CHECKSUM 2
/* seconds reset() will usleep() after it sends the commad, microseconds */
#define RESET_SLEEP 2000000

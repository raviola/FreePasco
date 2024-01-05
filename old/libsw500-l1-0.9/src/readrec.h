#ifndef readrec_h
#define readrec_h

/* FUNCTION prototypes: */
int readRecordCS(int openPort, char channels[10], short analog[3], unsigned short digital[2]);
int readRecordDE(int openPort, unsigned *timeOfEvent);
int readRecordMT(int openPort, unsigned short *timerValue, unsigned *timeOfEcho);
int readRecordTTO(int openPort, unsigned *timeStampOffset);
int readRecordType(int openPort);
/* END FUNCTION prototypes: */

#endif /* #ifndef readrec_h */

#ifndef ssetup_h
#define ssetup_h

/* FUNCTION prototypes: */
int inputSelect(int openPort, char channels[10]);
int logStateStore(int openPort, unsigned short numBytes, char *bytes);
int logStateRetrieve(int openPort, unsigned short *numBytes, char *bytes);
int rateSelect(int openPort, unsigned long samplePeriodIn, 
               unsigned long clockPeriodIn, unsigned short pingPeriodIn, 
               char smallBufferIn);
int rotaryMotRes(int openPort, char numPulses);
int triggerSelect(int openPort, unsigned char channel, unsigned char slope,
                                                            signed short level);
/* END FUNCTION prototypes: */

#endif /* #ifndef ssetup_h */

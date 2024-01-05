#ifndef scontrol_h
#define scontrol_h

/* FUNCTION prototypes: */
int checkBuffState(int openPort, unsigned short *dataBytes, unsigned char *loggedDataRuns);
int checkSampleState(int openPort);
int disableDataLogging(int openPort);
int enableDataLogging(int openPort);
int oneShotSample(int openPort);
int pauseSampling(int openPort);
int readBuff(int openPort);
int resetBuff(int openPort);
int resumeSampling(int openPort);
int startSampling(int openPort);
int stopSampling(int openPort);
/* END FUNCTION prototypes: */

#endif /* #ifndef scontrol_h */

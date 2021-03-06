// os345signals.h
#ifndef __os345signals_h__
#define __os345signals_h__

// ***********************************************************************
// Signals
#define SIG_SIGNAL(t,s)		sigSignal(t,s);

#define mySIGCONT			0x0001
#define mySIGINT			0x0002
#define mySIGKILL			0x0004
#define mySIGTERM			0x0008
#define mySIGTSTP			0x0010
#define mySIGSTOP			0x8000

int sigAction(void (*sigHandler)(void), int sig);
int sigSignal(int taskId, int sig);

int sigSignal(int taskId, int sig);
int sigAction(void (*sigHandler)(void), int sig);
void defaultSigIntHandler(void);
void defaultSigContHandler(void);
void defaultSigKillHandler(void);
void defaultSigTermHandler(void);
void defaultSigTstpHandler(void);
void createTaskSigHandlers(int tid);

int signals(void);

#endif // __os345signals_h__

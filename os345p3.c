// os345p3.c - Jurassic Park
// ***********************************************************************
// **   DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER   **
// **                                                                   **
// ** The code given here is the basis for the CS345 projects.          **
// ** It comes "as is" and "unwarranted."  As such, when you use part   **
// ** or all of the code, it becomes "yours" and you are responsible to **
// ** understand any algorithm or method presented.  Likewise, any      **
// ** errors or problems become your responsibility to fix.             **
// **                                                                   **
// ** NOTES:                                                            **
// ** -Comments beginning with "// ??" may require some implementation. **
// ** -Tab stops are set at every 3 spaces.                             **
// ** -The function API's in "OS345.h" should not be altered.           **
// **                                                                   **
// **   DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER   **
// ***********************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <time.h>
#include <assert.h>
#include "os345.h"
#include "os345park.h"
#include "DeltaClock.h"
#include "jparkTasks.h"

// ***********************************************************************
// project 3 variables

// Jurassic Park
extern JPARK myPark;
extern Semaphore* parkMutex;						// protect park access
extern Semaphore* fillSeat[NUM_CARS];			// (signal) seat ready to fill
extern Semaphore* seatFilled[NUM_CARS];		// (wait) passenger seated
extern Semaphore* rideOver[NUM_CARS];			// (signal) ride over
extern Semaphore* dcChange;
extern DeltaClock deltaClock;

Semaphore *get_passenger;
Semaphore *seat_taken;
Semaphore *passenger_seated;
Semaphore *temp_visitor_sem;
// create the rest of the semaphores and don't forget to initialize them!!! 
Semaphore *get_worker;
Semaphore *worker_queued;
Semaphore *worker_seated;
Semaphore *need_worker_mutex;
Semaphore *waiting_driver_ride;
Semaphore *waiting_driver_ticket;
Semaphore *waiting_driver_ride;
Semaphore *temp_driver_sem;
Semaphore *wake_up_driver;
Semaphore *give_ticket;

Semaphore *in_park;
Semaphore *ticket;
Semaphore *in_gift_shop;
Semaphore *in_museum;

int timeTaskID;
extern TCB tcb[];							// task control block
// ***********************************************************************
// project 3 functions and tasks
//void CL3_project3(int, char**);
//void CL3_dc(int, char**);


// ***********************************************************************
// ***********************************************************************
// project3 command
int P3_project3(int argc, char* argv[])
{
	srand(time(NULL));

	char buf[32];
	char* newArgv[2];

	// start park
	sprintf(buf, "jurassicPark");
	newArgv[0] = buf;
	createTask( buf,				// task name
		jurassicTask,				// task
		MED_PRIORITY,				// task priority
		1,								// task count
		newArgv);					// task argument

	// wait for park to get initialized...
	while (!parkMutex) SWAP;
	printf("\nStart Jurassic Park...");
	//?? create car, driver, and visitor tasks here
	//INITIALIZE SEMAPHORES
	get_passenger = createSemaphore("get passenger semaphore", BINARY, 0);					SWAP;
	seat_taken = createSemaphore("seat taken semaphore", BINARY, 0);						SWAP;
	passenger_seated = createSemaphore("passenger seated semaphore", BINARY, 0);			SWAP;
	get_worker = createSemaphore("Get Worker Semaphore", BINARY, 0);						SWAP;
	worker_queued = createSemaphore("Worker Queued Semaphore", BINARY, 0);					SWAP;
	worker_seated = createSemaphore("Worker Seated Semaphore", BINARY, 0);					SWAP;
	need_worker_mutex = createSemaphore("Need Worker Mutex", BINARY, 1);					SWAP;
	waiting_driver_ride = createSemaphore("Waiting Driver Ride Semaphore", BINARY, 0);		SWAP;
	waiting_driver_ticket = createSemaphore("Waiting Driver ticket Semaphore", BINARY, 0);	SWAP;
	wake_up_driver = createSemaphore("Wake up Driver Semaphore", BINARY, 0);				SWAP;
	give_ticket = createSemaphore("Give Ticket Semaphore", BINARY, 0);

	//counting semaphores
	in_park = createSemaphore("In Park Semaphore", COUNTING, MAX_IN_PARK);					SWAP;
	ticket = createSemaphore("Ticket Semaphore", COUNTING, MAX_TICKETS);					SWAP;
	in_gift_shop = createSemaphore("In Gift Shop Semaphore", COUNTING, MAX_IN_GIFTSHOP);	SWAP;
	in_museum = createSemaphore("In Museum Semaphore", COUNTING, MAX_IN_MUSEUM);			SWAP;
	
	for (int i = 0; i < NUM_CARS; i++)
	{
		char buf[8];																		SWAP;
		char buftwo[8];																		SWAP;
		sprintf(buf, "car%d", i);															SWAP;
		sprintf(buftwo, "%d", i);															SWAP;
		char* carArgv[] = { buf, buftwo };													SWAP;
		createTask(carArgv[0],				// task name
			car_task,				// task
			MED_PRIORITY,				// task priority
			2,								// task count
			carArgv); SWAP;					// task argument									
	}
	for (int i = 0; i < NUM_DRIVERS; i++)
	{
		char buf[8];																		SWAP;
		char buftwo[8];																		SWAP;
		sprintf(buf, "driver%d", i);														SWAP;
		sprintf(buftwo, "%d", i);															SWAP;
		char* driverArgv[] = { buf, buftwo };												SWAP;
		createTask(driverArgv[0],				// task name
			worker_task,				// task
			MED_PRIORITY,				// task priority
			2,								// task count
			driverArgv); SWAP;					// task argument
	}
	for (int i = 0; i < NUM_VISITORS; i++)
	{
		char buf[16];																		SWAP;
		char buftwo[8];																		SWAP;
		sprintf(buf, "visitor%d", i);														SWAP;
		sprintf(buftwo, "%d", i);															SWAP;
		char* visitorArgv[] = { buf, buftwo };												SWAP;
		createTask(visitorArgv[0],				// task name
			visitor_task,				// task
			MED_PRIORITY,				// task priority
			2,								// task count
			visitorArgv); SWAP;					// task argument
	}
	//myPark.numInCarLine = myPark.numInPark = 4;
	return 0;
} // end project3

// ***********************************************************************
// display all pending events in the delta clock list
void printDeltaClock(void)
{
	int i;
	for (i = 0; i < deltaClock.size; i++)
	{
		printf("\n%4d%4d  %-20s", i, deltaClock.items[i].time_to_run, deltaClock.items[i].event->name);
	}
	return;
}


// ***********************************************************************
// ***********************************************************************
// delta clock command
int P3_dc(int argc, char* argv[])
{
	printf("\nDelta Clock");
	// ?? Implement a routine to display the current delta clock contents
	printDeltaClock();
	return 0;
} // end CL3_dc


// ***********************************************************************
// ***********************************************************************
// ***********************************************************************
// ***********************************************************************
// ***********************************************************************
// ***********************************************************************
// delta clock command
//int P3_dc(int argc, char* argv[])
//{
//	printf("\nDelta Clock");
//	// ?? Implement a routine to display the current delta clock contents
//	//printf("\nTo Be Implemented!");
//	int i;
//	for (i=0; i<numDeltaClock; i++)
//	{
//		printf("\n%4d%4d  %-20s", i, deltaClock[i].time, deltaClock[i].sem->name);
//	}
//	return 0;
//} // end CL3_dc




//// ***********************************************************************
//// monitor the delta clock task
int dcMonitorTask(int argc, char* argv[])
{
	int i, flg;													SWAP;
	char buf[32];												SWAP;
	Semaphore *event[10];										SWAP;
	// create some test times for event[0-9]
	int ttime[10] = {
		90, 300, 50, 170, 340, 300, 50, 300, 40, 110	};		SWAP;

	for (i=0; i<10; i++)
	{
		sprintf(buf, "event[%d]", i);							SWAP;
		event[i] = createSemaphore(buf, BINARY, 0);				SWAP;
		add_to_delta_clock(&deltaClock, ttime[i], event[i]);	SWAP;
	}
	printDeltaClock();											SWAP;

	while (deltaClock.size > 0)
	{
		SEM_WAIT(dcChange);										SWAP;
		flg = 0;												SWAP;
		for (i=0; i<10; i++)
		{
			if (event[i]->state ==1)			
			{
					printf("\n  event[%d] signaled", i);		SWAP;
					event[i]->state = 0;						SWAP;
					flg = 1;									SWAP;
			}
		}
		if (flg) printDeltaClock();								SWAP;
	}
	printf("\nNo more events in Delta Clock");					SWAP;

	// kill dcMonitorTask
	tcb[timeTaskID].state = S_EXIT;								SWAP;
	return 0;
} // end dcMonitorTask


extern Semaphore* tics1sec;

// ********************************************************************************************
// display time every tics1sec
int timeTask(int argc, char* argv[])
{
	char svtime[64];						// ascii current time
	while (1)
	{
		SEM_WAIT(tics1sec);										SWAP;
		printf("\nTime = %s", myTime(svtime));					SWAP;
	}
	return 0;
} // end timeTask

extern Semaphore* tics10thsec;

//int deltaClockDecrement(int argc, char *argv[])
//{
//	while (1)
//	{
//		SEM_WAIT(tics10thsec);									SWAP;
//		decrement_delta_clock(&deltaClock);						SWAP;
//		SEM_SIGNAL(dcChange);									SWAP;
//	}
//
//}


// ***********************************************************************
// test delta clock
int P3_tdc(int argc, char* argv[])
{
	createTask("DC Test",			// task name
		dcMonitorTask,		// task
		10,					// task priority
		argc,					// task arguments
		argv);

	timeTaskID = createTask("Time",		// task name
		timeTask,	// task
		10,			// task priority
		argc,			// task arguments
		argv);

	//createTask("DeltaClock",
	//			deltaClockDecrement,
	//			HIGHEST_PRIORITY,
	//			argc,
	//			argv);
	
	return 0;
} // end P3_tdc



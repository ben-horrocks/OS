#ifndef DELTACLOCK_H
#define DELTACLOCK_H

#include "os345.h"
extern Semaphore *dcChange;

#define MAX_CLOCK_ITEMS 128
typedef Semaphore semaphore;
Semaphore* delta_clock_semaphore;
typedef struct
{
	int time_to_run;
	semaphore* event;
} delta_clock_item;

typedef struct
{
	int size;
	Semaphore* delta_clock_semaphore;
	delta_clock_item items[128];
} DeltaClock;

DeltaClock initialize_delta_clock();
int add_to_delta_clock(DeltaClock* clock, int time_to_run, Semaphore* sem);
void decrement_delta_clock(DeltaClock* clock);
//void testdeltaclock();

#endif


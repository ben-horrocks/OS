#include "DeltaClock.h"
#include <stddef.h>

DeltaClock initialize_delta_clock()
{
	DeltaClock clock;
	clock.delta_clock_semaphore = createSemaphore("deltaClock", BINARY, 1);
	clock.size = 0;
	return clock;
}

int add_to_delta_clock(DeltaClock* clock, int time_to_run, Semaphore* sem)
{
	SEM_WAIT(clock->delta_clock_semaphore);					SWAP;
	if (clock->size == MAX_CLOCK_ITEMS)						
	{
		printf("Delta Clock Full");							SWAP;
		SEM_SIGNAL(clock->delta_clock_semaphore);			SWAP;
		return -1;
	}
	int i = 0;												SWAP;
	for (i = 0; i <= clock->size; i++)
	{
		if (clock->size == i)
		{
			break;											
		}
		else if (clock->items[i].time_to_run <= time_to_run)
		{
			time_to_run -= clock->items[i].time_to_run;		SWAP;
		}
		else
		{
			break;
		}
	}
	int temp = clock->size;									SWAP;
	while (temp != i)
	{
		clock->items[temp] = clock->items[temp - 1];		SWAP;
		temp--;												SWAP;
	}
	delta_clock_item item = { time_to_run, sem };			SWAP;
	clock->items[i] = item;									SWAP;
	if(i != clock->size)
	{
		clock->items[i + 1].time_to_run -= time_to_run;		SWAP;
	}
	clock->size++;											SWAP;
	SEM_SIGNAL(clock->delta_clock_semaphore);				SWAP;
	return 0;
}

void decrement_delta_clock(DeltaClock* clock)
{
	if (clock->size == 0 || clock->delta_clock_semaphore->state != 1)
	{
		return;
	}
	clock->items[0].time_to_run = clock->items[0].time_to_run < 0? 0: clock->items[0].time_to_run - 1;
	while (clock->items[0].time_to_run == 0)
	{
		if (clock->items[0].time_to_run < 0)
		{
			printf("Got a negative thing\n");
		}
		SEM_SIGNAL(clock->items[0].event);					
		SEM_SIGNAL(dcChange);								
		int temp = 0;										
		while (temp < clock->size - 1)
		{
			clock->items[temp] = clock->items[temp + 1];	
			temp++;											
		}
		clock->items[temp].time_to_run = -1;				
		clock->items[temp].event = 0;						
		clock->size--;										
	}
}

//void testdeltaclock()
//{
//	DeltaClock clock = initialize_delta_clock();
//	add_to_delta_clock(&clock, 5, 111);
//	add_to_delta_clock(&clock, 10, 112);
//	add_to_delta_clock(&clock, 8, 113);
//	add_to_delta_clock(&clock, 12, 114);
//	add_to_delta_clock(&clock, 2, 115);
//	add_to_delta_clock(&clock, 15, 116);
//	for (int x = 0; x < 15; x++)
//	{
//		decrement_delta_clock(&clock);
//	}
//}


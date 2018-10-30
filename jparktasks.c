#include "jparkTasks.h"
#include "os345.h"
#include "os345park.h"
#include "DeltaClock.h"

extern JPARK myPark;
extern Semaphore* parkMutex;						// protect park access
extern Semaphore* fillSeat[NUM_CARS];			// (signal) seat ready to fill
extern Semaphore* seatFilled[NUM_CARS];		// (wait) passenger seated
extern Semaphore* rideOver[NUM_CARS];			// (signal) ride over
extern DeltaClock deltaClock;

extern Semaphore *get_passenger;
extern Semaphore *seat_taken;
extern Semaphore *passenger_seated;
extern Semaphore *temp_visitor_sem;
// create the rest of the semaphores and don't forget to initialize them!!! 
extern Semaphore *get_worker;
extern Semaphore *worker_queued;
extern Semaphore *worker_seated;
extern Semaphore *need_worker_mutex;
extern Semaphore *waiting_driver_ticket;
extern Semaphore *waiting_driver_ride;
extern Semaphore *temp_driver_sem;
extern Semaphore *wake_up_driver;
extern Semaphore *give_ticket;

extern Semaphore *in_park;
extern Semaphore *ticket;
extern Semaphore *in_gift_shop;
extern Semaphore *in_museum;

int car_that_signaled = -1;

int car_task(int argc, char*argv[])
{
	int carID = atoi(argv[1]);					SWAP;
	while (1)
	{
		Semaphore *riders[3];					SWAP;
		Semaphore *driver;						SWAP;
		for (int i = 0; i < 3; i++)
		{
			SEM_WAIT(fillSeat[carID]);			SWAP;
			SEM_SIGNAL(get_passenger);			SWAP;
			SEM_WAIT(seat_taken);				SWAP;
			//get passenger
			riders[i] = temp_visitor_sem;		SWAP;
			SEM_SIGNAL(passenger_seated);		SWAP;
			SEM_SIGNAL(seatFilled[carID]);		SWAP;
		}
		//get driver
		SEM_WAIT(need_worker_mutex);			SWAP;
		SEM_SIGNAL(waiting_driver_ride);		SWAP;
		car_that_signaled = carID;
		SEM_SIGNAL(wake_up_driver);				SWAP;
		SEM_WAIT(worker_queued);				SWAP;
		driver = temp_driver_sem;				SWAP;
		SEM_SIGNAL(worker_seated);				SWAP;
		SEM_SIGNAL(need_worker_mutex);			SWAP;


		SEM_WAIT(rideOver[carID]);				SWAP;

		SEM_SIGNAL(driver);						SWAP;
		for (int i = 0; i < 3; i++)
		{
			SEM_SIGNAL(riders[i]);				SWAP;
		}

	}
	return 0;
}

int worker_task(int argc, char*argv[])
{

	int driverID = atoi(argv[1]);										SWAP;
	Semaphore *driver = createSemaphore(argv[0], BINARY, 0);			SWAP;
	while (1)
	{
		SEM_WAIT(parkMutex);											SWAP;
		myPark.drivers[driverID] = 0;									SWAP;
		SEM_SIGNAL(parkMutex);											SWAP;
		SEM_WAIT(wake_up_driver);										SWAP;
		//check if need to serve tickets
		//printf("Driver is awake\n");
		if (SEM_TRYLOCK(waiting_driver_ride))
		{
			//printf("Need a ride?\n");
			SEM_WAIT(parkMutex);										SWAP;
			myPark.drivers[driverID] = car_that_signaled + 1;			SWAP;
			SEM_SIGNAL(parkMutex);										SWAP;
			temp_driver_sem = driver;									SWAP;
			SEM_SIGNAL(worker_queued);									SWAP;
			SEM_WAIT(worker_seated);									SWAP;
			SEM_WAIT(driver);											SWAP;
		}
		//check if need driver
		else if (SEM_TRYLOCK(waiting_driver_ticket))
		{
			//printf("Need a ticket?\n");
			SEM_WAIT(parkMutex);										SWAP;
			myPark.drivers[driverID] = -1;								SWAP;
			SEM_SIGNAL(parkMutex);										SWAP;
			SEM_WAIT(ticket);											SWAP;
			SEM_SIGNAL(give_ticket);									SWAP;
		}
		else
		{
			break;
		}

	}
	return 0;
}

int visitor_task(int argc, char*argv[])
{
	//printf("Visitor task starting\n");
	const TEN_SECONDS = 100;														SWAP;
	const FIVE_SECONDS = 50;														SWAP;
	int time_to_next_activity;														SWAP;
	int visitorID = atoi(argv[1]);													SWAP;
	//printf("Creating Semaphore\n");
	Semaphore *my_timing_semaphore = createSemaphore(argv[0], BINARY, 0);			SWAP;
	//printf("Created Semaphore\n");
	//time to arrive at park
	time_to_next_activity = rand() % TEN_SECONDS;									SWAP;
	//printf("adding to clock\n");
	add_to_delta_clock(&deltaClock, time_to_next_activity, my_timing_semaphore);	SWAP;
	//printf("Semaphore: %s", my_timing_semaphore->name);								SWAP;
	SEM_WAIT(my_timing_semaphore);													SWAP;
	
	// enter park		
	//printf("Entering Park\n");
	SEM_WAIT(parkMutex);															SWAP;
	myPark.numOutsidePark += 1;														SWAP;
	SEM_SIGNAL(parkMutex);															SWAP;
	//printf("In Park\n");
	SEM_WAIT(in_park);																SWAP;
	SEM_WAIT(parkMutex);															SWAP;
	myPark.numOutsidePark -= 1;														SWAP;
	myPark.numInPark += 1;															SWAP;
	// stand in line for ticket
	myPark.numInTicketLine += 1;													SWAP;
	SEM_SIGNAL(parkMutex);															SWAP;
	// need to implement getting a ticket from a driver
	SEM_WAIT(need_worker_mutex);													SWAP;
	//printf("Need a worker to give me a ticket!\n");
	SEM_SIGNAL(waiting_driver_ticket);												SWAP;
	//printf("Told a worker to give me a ticket!\n");
	SEM_SIGNAL(wake_up_driver);														SWAP;
	//printf("Wake Up Driver!\n");
	SEM_WAIT(give_ticket);															SWAP;
	SEM_SIGNAL(need_worker_mutex);													SWAP;
		//SEM_WAIT(ticket);																SWAP;
	SEM_WAIT(parkMutex);															SWAP;
	myPark.numInTicketLine -= 1;													SWAP;
	myPark.numTicketsAvailable -= 1;												SWAP;
	// go to museum
	myPark.numInMuseumLine += 1;													SWAP;
	SEM_SIGNAL(parkMutex);															SWAP;
	SEM_WAIT(in_museum);
	SEM_WAIT(parkMutex);															SWAP;
	myPark.numInMuseumLine -= 1;													SWAP;
	myPark.numInMuseum += 1;														SWAP;
	SEM_SIGNAL(parkMutex);															SWAP;
	time_to_next_activity = rand() % FIVE_SECONDS;									SWAP;
	add_to_delta_clock(&deltaClock, time_to_next_activity, my_timing_semaphore);	SWAP;
	SEM_WAIT(my_timing_semaphore);													SWAP;
	SEM_SIGNAL(in_museum);															SWAP;
	SEM_WAIT(parkMutex);															SWAP;
	myPark.numInMuseum -= 1;														SWAP;
	// ride the ride
	myPark.numInCarLine += 1;														SWAP;
	myPark.numTicketsAvailable += 1;												SWAP;
	SEM_SIGNAL(parkMutex);															SWAP;
	SEM_WAIT(get_passenger);														SWAP;
	SEM_SIGNAL(ticket);																SWAP;
	SEM_WAIT(parkMutex);															SWAP;
	myPark.numInCarLine -= 1;														SWAP;
	myPark.numInCars += 1;
	SEM_SIGNAL(parkMutex);															SWAP;
	temp_visitor_sem = my_timing_semaphore;											SWAP;
	SEM_SIGNAL(seat_taken);															SWAP;
	SEM_WAIT(my_timing_semaphore);													SWAP;
	// go to gift shop
	SEM_WAIT(parkMutex);															SWAP;
	myPark.numInCars -= 1;															SWAP;
	myPark.numInGiftLine += 1;														SWAP;
	SEM_SIGNAL(parkMutex);															SWAP;
	SEM_WAIT(in_gift_shop);															SWAP;
	SEM_WAIT(parkMutex);															SWAP;
	myPark.numInGiftLine -= 1;														SWAP;
	myPark.numInGiftShop += 1;														SWAP;
	SEM_SIGNAL(parkMutex);															SWAP;
	time_to_next_activity = rand() % FIVE_SECONDS;									SWAP;
	add_to_delta_clock(&deltaClock, time_to_next_activity, my_timing_semaphore);	SWAP;
	SEM_WAIT(my_timing_semaphore);													SWAP;
	// leave park
	SEM_WAIT(parkMutex);															SWAP;
	myPark.numInGiftShop -= 1;														SWAP;
	myPark.numInPark -= 1;															SWAP;
	myPark.numExitedPark += 1;														SWAP;
	SEM_SIGNAL(parkMutex);															SWAP;
	SEM_SIGNAL(in_gift_shop);														SWAP;
	SEM_SIGNAL(in_park);															SWAP;
	return 0;
}
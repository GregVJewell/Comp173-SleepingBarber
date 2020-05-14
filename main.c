#define MAX (rand() % (30 - 5 + 1)) + 5
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// Declarations
void *Customer(void *param);
void *barber(void *param);

// Locks
sem_t seat_mutex;
sem_t sem_customer;
sem_t sem_barber;

// Global Variables
int seats;
int waitTime;

void *barber(void *param) {
   int worktime;
  
   while(1) {
	  sem_wait(&sem_customer);
	  sem_wait(&seat_mutex);
	  seats += 1;
	  printf("Barber: Taking a Customer. Seats Available = %d\n", seats);
	  sem_post(&sem_barber);
	  sem_post(&seat_mutex);

	  worktime = (rand() % 4) + 1;

	  printf("Barber: Cutting hair for %d seconds\n", worktime);
	  sleep(worktime);
    } 
}

void *Customer(void *param) {
   int waittime;

   while(1) {
	  sem_wait(&seat_mutex);
    // Check for Open Seats
	  if(seats <= 0){
		   printf("Customer: Thread %u No Seats Available\n", (unsigned int)pthread_self());
		   sem_post(&seat_mutex);
	  }
    // Lock, Supply Seat, Unlock
	  else{
		   seats -= 1;
		   printf("Customer: Thread %u taking a seat.\n",(unsigned int)pthread_self());
		   sem_post(&sem_customer);
		   sem_post(&seat_mutex);
		   sem_wait(&sem_barber);
		   printf("Customer: Thread %u moving to barber\n",(unsigned int)pthread_self());
	  }
	  waittime = (rand() % waitTime) + 1;
	  sleep(waittime);
    }
}

int main(int argc, char *argv[]) {
	pthread_t barberid;
	pthread_t Customerids[MAX];
	
	printf("\n------Main thread beginning------\n\n");
   /* 1. Get command line arguments */
   int hours,customers,i;

   // User input failure!
   if (argc != 3){
	   printf("Please enter 2 arguments: <Program run time> <Number of seats>\n");
	   exit(0);
   }

   // Convert User Input
   hours = atoi(argv[1]);
   seats = atoi(argv[2]);

   // Set Core Variables
   customers = (rand() % (30 - 5 + 1) + 5);
   waitTime = (rand() % (100 - 10 + 1) + 10);

   if(hours < 1){
     printf("Hours must be greater than 0!!!");
     exit(0);
   }

   if(seats < 1){
     printf("Seats available must be greater than 0!");
     exit(0);
   }

   // Initialize semaphores
   sem_init(&seat_mutex,0,1);
   sem_init(&sem_customer,0,0);
   sem_init(&sem_barber,0,0);

   // Create barber thread.
   pthread_create(&barberid, NULL, barber, NULL);
   printf("Creating barber thread with id %lu\n",barberid);

   // Create Customer threads.
   for (i = 0; i < customers; i++){
	   pthread_create(&Customerids[i], NULL, Customer, NULL);
	   printf("Creating Customer thread with id %lu\n",Customerids[i]);
   }

   // Sleep
   printf("Main thread sleeping for %d seconds\n", hours);
   sleep(hours);

   // Exit
   printf("\n------Main thread exiting------\n");
   exit(0);
}
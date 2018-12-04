#include<stdio.h>
#include<pthread.h>
#include<time.h>
#include<stdlib.h>
#include<unistd.h>


void *Pick_fork ( int );
void *StartEating ( void *arg );
void *test ( int );
void *Drop_Fork ( int );

int num = 5;			//5 PEOPLE					     
double hungrytimes [5];
int times_eat = 0;
int eatnum = 1;							
					
enum { think, hungry, eat } PhilStates [5]; // THE 3 STATES
pthread_mutex_t lock;								
pthread_cond_t ME [5];							
pthread_t philosopher [5];
int philrunning [5] = {0,1,2,3,4};



//MAIN PROGRAM
int main ( ){	
	pthread_mutex_init ( &lock, NULL );
	int a ;
	for ( a = 0; a < num; a++ )	{
	PhilStates[a] = think;
	pthread_cond_init ( &ME [a], NULL );
		hungrytimes [a] = 0.0; 
	}
									
	for (a=0; a<num; a++)
	{	pthread_create (&philosopher[a],NULL,StartEating,&philrunning [a]);	
}

	for (a=0; a<num; a++)						 	
	{	pthread_join (philosopher[a],NULL);						

		printf ( "Philosopher %d is done eating for now \n", a+1 );}

	printf ("The ave waiting times are: \n" );
	for (a=0; a<num; a++ )			//PRINT THE TIME THE PHILOSOPHRES WERE WAITING
{	printf ("Philosopher No.%d : %f \n", a+1, hungrytimes[a] );

	}

	return 0;
}


//FUNCTIONS START HERE
//PHILOSOPHER PICKS UP FORK
void *Pick_fork ( int number )
{
	pthread_mutex_lock ( &lock );						
	PhilStates [number] = hungry;
	pthread_mutex_unlock ( &lock );
	test(number);								 
	pthread_mutex_lock ( &lock );
	if (PhilStates [number] != eat )
	{pthread_cond_wait ( &ME[number], &lock );			
	}
	pthread_mutex_unlock (&lock);
	return NULL;
}

//PHILOSOPHER DROPS THE FORK FOR OTHERS
void *Drop_Fork ( int number )
{
	pthread_mutex_lock ( &lock );						
	PhilStates [ number ] = think;
	pthread_mutex_unlock ( &lock );
	test ((number + num-1)%num );					
	test ((number+1)%num );
}

//EATING FUNCTYION
void *StartEating ( void *arg )
{	
int *tmp_ptr = (int*)arg;
int number = *tmp_ptr;
double Total=0.0;
time_t start, end;
double wait;
printf ("Philospher %d started eating \n",number + 1);
time_t t;
srand ((unsigned)time(&t));

	int iter;
	for (  iter = 0; iter < eatnum; iter++ )
{printf ( "Eating number = %d \n", times_eat );
	int Rand = ( rand())%2 + 1;							
	printf ( "Philospher %d thinking for %d seconds. \n",number+1,Rand );
		sleep(Rand );
		time(&start );
		Pick_fork(number );									
		time(&end);
		wait=difftime(end,start);					
		printf( "Philospher %d goes hungry for %f seconds. \n", number + 1,wait);


		int randtime =( rand())%2 + 1;								
		sleep(randtime);
		printf ( "Philospher %d eating at %d th time for %d seconds. \n",number + 1,iter+1,randtime );
		Drop_Fork ( number );									
		times_eat++;
		Total= Total+wait; 
	}
	hungrytimes[number]=Total/200.00;
return NULL;
}

//AVOID DEADLOCKS
void *test (int number )
{	int check = (number%2 == 0);						// old implementation to avoid deadlock
	int first= (number+num-1)% num;
	int second= ( number + 1 ) % num;
	if ( check )
	{pthread_mutex_lock( &lock );
		if ( (PhilStates[first] != eat) && (PhilStates[second] != eat) && (PhilStates[number] == hungry) )
		{
			PhilStates[number] = eat;
			pthread_cond_signal ( &ME [number] );
		}pthread_mutex_unlock( &lock );}
	else{
		pthread_mutex_lock( &lock );
		if ( (PhilStates[first] != eat) && (PhilStates[second]!=eat) && (PhilStates[number]==hungry) )
		{
			PhilStates[number] = eat;
			pthread_cond_signal ( &ME [number] );
		}
		pthread_mutex_unlock( &lock );

	}

}

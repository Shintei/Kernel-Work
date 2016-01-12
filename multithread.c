#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#define PTHREAD_SYNC

int SharedVariable = 0;
#ifdef PTHREAD_SYNC
  pthread_mutex_t mutex;
  pthread_barrier_t barr;
#endif

void *SimpleThread(void* which)
{
   int num, val;
   
   for(num = 0; num < 20; num++)
   {
	if(random() > RAND_MAX/2)
	{
	    usleep(10);
	}
	
        #ifdef PTHREAD_SYNC     
	   pthread_mutex_lock(&mutex);
        #endif
	val = SharedVariable;
	printf("*** thread %d sees value %d\n", (int) which, val);
	SharedVariable = val + 1;
	#ifdef PTHREAD_SYNC
	   pthread_mutex_unlock(&mutex);
      	   pthread_barrier_wait(&barr);
        #endif
   }
   val = SharedVariable;
   printf("Thread %d sees final value %d\n", (int) which, val);
}

int main(int argc, char *argv[])
{
    if((argc != 2) || atoi(argv[1]) < 1)
    {
	printf("Must have 2 arguments and have an integer argument for 2nd argument\n");
	return -1;
    }
    else
    {
	int threadret;
	int NUM_OF_THREADS = atoi(argv[1]);
	pthread_t totalthreads [NUM_OF_THREADS];
	int i;
	int j;

        #ifdef PTHREAD_SYNC
           pthread_mutex_init(&mutex, NULL);
	   pthread_barrier_init(&barr, NULL, NUM_OF_THREADS);
        #endif

	for(i = 0; i < NUM_OF_THREADS; i++)
	{
           threadret = pthread_create(&totalthreads[i], NULL, SimpleThread, (void *) i);
	}
	#ifdef PTHREAD_SYNC
	   for(j = 0; j < NUM_OF_THREADS; j++)
	   {
		pthread_join(totalthreads[j], NULL);
	   }
	#endif

	pthread_exit(NULL);
	
	return 0;	
    }
	
	

}

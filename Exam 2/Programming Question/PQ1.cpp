#include <pthread.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <cmath>

static pthread_mutex_t bsem;    // Mutex semaphore
static pthread_cond_t waitTurn = PTHREAD_COND_INITIALIZER;  // Condition variable to control the turn
static int turn; // Index to control access to the turn array
static int maxEvenThread;
static int *arr;


void *printEvenOdd(void *void_ptr_argv)
{
    int myTurn = *(int *) void_ptr_argv;

    pthread_mutex_lock(&bsem);

    while(myTurn != arr[turn]) {
        pthread_cond_wait(&waitTurn, &bsem);
    }

    pthread_mutex_unlock(&bsem);

    // if it's my turn, print and increment turn
    pthread_mutex_lock(&bsem);

    std::cout << "I am child thread " << arr[turn] << std::endl;
    
    turn++;

    pthread_cond_broadcast(&waitTurn);

    pthread_mutex_unlock(&bsem);
    
    return nullptr;
}

int main()
{
    int nThreads;
    //std::cin >> nThreads;   // number of Threads from STDIN
    nThreads = 5;
    
    pthread_mutex_init(&bsem, NULL); // Initialize bsem to 1
 	
 	pthread_t *tid= new pthread_t[nThreads];
	int *threadNumber = new int[nThreads];
	turn = 0; // initialize the turn here
    arr = new int[nThreads];
	
	// Determine the max thread number for even threads
	if ((nThreads - 1)%2 == 0) {
	    maxEvenThread = nThreads - 1;
    }
	else {
	    maxEvenThread = nThreads - 2;
    }

    for(int i = 0; i < maxEvenThread - 1; i++) {
        arr[i] = i*2;
    }

    int num = -1;
    for(int i = 0; i < nThreads; i++) {
        if(i % 2 == 1) {
            arr[maxEvenThread + num] = i;
            num++;
        }
    }

	for(int i=0;i<nThreads;i++)
	{
	   
        threadNumber[i] = i; // initialize the thread number here (remember to follow the rules from the specifications of the assignment)
        // call pthread_create here
        if(pthread_create(&tid[i], nullptr, printEvenOdd, &threadNumber[i])) {
            std::cout << "Failed to create thread\n";
            return 1;
        }
	}
	// Call pthread_join
    for(int i = 0; i < nThreads; i++) {
        pthread_join(tid[i], NULL);
    }
	
	
    delete [] threadNumber;
    delete [] tid;
	return 0;
}
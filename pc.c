/*
    Erik Safford
    Producer-Consumer Problem with Threads/Semaphores
    Spring 2020
*/
//compile with -pthread (threads) -lpthread -lrt (semaphores)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>

//Number of spaces within buffer_item
#define BUFFER_SIZE 5

//Thread mutex for critical sections of inserting/removing from buffer
pthread_mutex_t mutex;

//Process semaphores for telling if the buffer is empty/full
sem_t empty, full;

//Buffer for producers and consumers to access
typedef int bufferItem;
bufferItem buffer[BUFFER_SIZE];
int bufferIndex;

//Flag to catch when Ctrl-c is entered or sleepTime expires, causing all producers/consumers to exit
bool interruptFlag = false;

//Signal handler to catch Ctrl-c (SIGINT), sets interruptFlag to true
void handleCtrlC() {
    interruptFlag = true;
    printf(" Ctrl-c received, attempting to exit...\n");
}

//Insert item into the buffer
int insertItem(bufferItem item) {
    //return 0 if successful, otherwise return -1 indicating error condition
    if(bufferIndex < BUFFER_SIZE) {
        buffer[bufferIndex] = item;
        bufferIndex++;
        return(0);
    }
    else {
        return(-1);
    }
}

//Remove an object from buffer placing it in item
int removeItem() {
    //return 0 if successful, otherwise return -1 indicating error condition
    bufferItem item;
    if(bufferIndex > 0) {
        bufferIndex--;
        item = buffer[bufferIndex];
        return(0);
    }
    else {
        return(-1);
    }
}

//Producer sleeps for random time, then inserts random number item into buffer
void *producerFunc(void *producerNumber) {
    //Seed random
    srand(time(0) % 10);

    //Attempt to produce until interruptFlag is set to true
    while(!interruptFlag) {
        //Sleep for random time
        sleep(rand() % 10);

        //Get random item value to enter into buffer
        bufferItem item = rand() % 50;

        //Decrement empty semaphore, making it so there is one less available space in buffer for other producers
        if(sem_wait(&empty) != 0) {
            perror("Producer sem_wait error");
        }

        //Lock mutex, only one thread can get past
        if(pthread_mutex_lock(&mutex) != 0) {
            perror("Producer pthread_mutex_lock error");
        }  
        
        //Insert item into buffer
        int ret = insertItem(item);
        if(ret == 0) {
            printf("Producer thread %d produced %d\n", *(int *)producerNumber, item);
        }
        else if(ret == -1) {
            printf("Full buffer\n");
        }

        //Unlock mutex, other threads can now use
        if(pthread_mutex_unlock(&mutex) != 0) {
            perror("Producer pthread_mutex_unlock error");
        } 

        //Increment full semaphore, signaling consumer that there is an item to be consumed
        if(sem_post(&full) != 0) {
            perror("Producer sem_post error");
        }
    }
    return(NULL);
}

//Consumer sleeps for random time, then consumes number item out of the buffer
void *consumerFunc(void *consumerNumber) {
    //Seed random
    srand(time(0) % 10);

    //Attempt to consume until interruptFlag is set to true
    while(!interruptFlag) {
        //Sleep for random time
        sleep(rand()%10);
  
        //Decrement full semaphore, signaling other consumers to wait
        if(sem_wait(&full) != 0) {
            perror("Consumer sem_wait error");
        }

        //Lock mutex, only one thread can get past
        if(pthread_mutex_lock(&mutex) != 0) {
            perror("Consumer pthread_mutex_lock error");
        }
 
        //Remove item from buffer
        int ret = removeItem();
        if(ret == 0) {
            printf("Consumer thread %d consumed %d\n", *(int *)consumerNumber, buffer[bufferIndex]);
        }
        else if(ret == -1) {
            printf("Empty buffer\n");
        }

        //Unlock thread mutex, other threads can now use
        if(pthread_mutex_unlock(&mutex) != 0) {
            perror("Consumer pthread_mutex_unlock error");
        }

        //Increment empty semaphore, making it so there is one more available space in buffer for producers
        if(sem_post(&empty) != 0) {
            perror("Consumer sem_post error");
        } 
    }
    return(NULL);
}

int main(int argc, char **argv) {
    //Check for proper number of command line args
    if(argc != 4) {
        printf("./pc howLongToSleep numberOfProducers numberOfConsumers\n");
        exit(1);
    }

    /* 1. Get command line arguments argv[1], argv[2], argv[3] */

    char *ptr;
    //Get how long to sleep before terminating
    long sleepTime;
    if((sleepTime = strtol(argv[1], &ptr, 10)) == 0) {
        fprintf(stderr, "sleepTime must be a postive numeric value\n");
        exit(1);
    }
    //Get how many producer threads to create
    long numProducers;
    if((numProducers = strtol(argv[2], &ptr, 10)) == 0) {
        fprintf(stderr, "numProducers must be a postive numeric value\n");
        exit(1);
    }
    //Get how many consumer threads to create
    long numConsumers;
    if((numConsumers = strtol(argv[3], &ptr, 10)) == 0) {
        fprintf(stderr, "numConsumers must be a postive numeric value\n");
        exit(1);
    }
    printf("Waiting for %ld seconds before exiting, with %ld producers and %ld consumers...\n", sleepTime, numProducers, numConsumers);

    /* 2. Initialize the buffer, threads, thread mutex, and process semaphores */

    //bufferIndex starts at 0 (empty)
    bufferIndex = 0;
    //Create pthread array to help create numProducers producers and numConsumers consumers
    pthread_t threads[numProducers + numConsumers];
    //Initialize thread mutex with defaults
    if(pthread_mutex_init(&mutex, NULL) != 0) {
        perror("main pthread_mutex_init error");
    }
    //empty semaphore initialized to BUFFER_SIZE so numProducers producers can add BUFFER_SIZE elements to buffer
    if(sem_init(&empty, 0, BUFFER_SIZE) != 0) {
        perror("main sem_init error (empty)");
    }
    //full semaphore initialized to 0 so producer can increment once buffer is ready to be consumed by consumer
    if(sem_init(&full, 0, 0) != 0) {
        perror("main sem_init error (full)");
    }

    /* 3. Create producer thread(s) */
    /* 4. Create consumer thread(s) */

    //Create int array to keep track of producer and consumer threads
    int threadNumber[numProducers + numConsumers];
    //Create producer and consumer threads
    for(int i = 0; i < numProducers + numConsumers; i++) {
        threadNumber[i] = i;
        if(i < numProducers) {
            if(pthread_create(&threads[i], NULL, producerFunc, (void *)&threadNumber[i]) != 0) {
                perror("main pthread_create error (producer)");
            }
        }
        else {
            if(pthread_create(&threads[i], NULL, consumerFunc, (void *)&threadNumber[i]) != 0) {
                perror("main pthread_create error (consumer)");
            }
        }
    }
	
    /* 5. Set the signal handler and sleep for a sleepTime period of time */

    if(signal(SIGINT, handleCtrlC) == SIG_ERR) {
        perror("main signal error");
    }
    sleep(sleepTime);
    
    /* 6. Upon awakening, tell threads to cancel, join, and then exit the application */

    //Set interrupt flag, which breaks consumers/producers from their infinite loops
    interruptFlag = true;
    //Tell all threads to cancel (deffered cancellation)
    for(int i = 0; i < numProducers + numConsumers; i++) {
        if(pthread_cancel(threads[i]) != 0) {
            perror("main pthread_cancel error");
        } 
    }
    //Wait until all threads have exited (joined)
    for(int i = 0; i < numProducers + numConsumers; i++) {
        if(pthread_join(threads[i], NULL) != 0) {
            perror("main pthread_join error");
        } 
    }
    printf("All threads joined, destroying mutexes/semaphores...\n");
    //Destroy semaphores and mutexes
    if(sem_destroy(&empty) != 0) {
        perror("main sem_destroy error (empty)");
    }
    if(sem_destroy(&full) != 0) {
        perror("main sem_destroy error (full)");
    }
    if(pthread_mutex_destroy(&mutex) != 0) {
        perror("main pthread_mutex_destroy error");
    }
    return(0);
}

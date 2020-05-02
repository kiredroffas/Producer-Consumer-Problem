# Producer-Consumer-Problem
* The Producer-Consumer problem is as follows: 
    * We have a buffer of fixed size. A producer can produce an item and can place it in the buffer. A consumer can pick items from the buffer and can consume them. We need to ensure that when a producer is placing an item in the buffer, then at the same time consumer should not consume any item. In this problem, the buffer is the critical section.
* This program implements the Producer-Consumer problem in C using process semaphores and a thread mutex. 
* Standard counting semaphores are used for 'empty' and 'full', and a mutex lock is used to represent 'mutex'. The producer and consumer (running as separate threads) move items to and from a buffer that is synchronized with the 'empty', 'full', and 'mutex' structures. This is done through the use of Linux Pthreads.
* The buffer consists of a fixed-size array of type 'bufferItem', which is defined using a typedef. The array of 'bufferItem' objects are manipulated as a stack. The buffer is manipulated through two functions: 'insertItem()' and 'removeItem(), which are called by the producer and consumer threads. The 'insertItem()' and 'removeItem()' functions are synchronized through the process semaphores and thread mutex, which are initialized before the threads are created.
* The 'main()' function initializes the buffer and creates the separate producer and consumer threads. Once it has created the producer and consumer threads, the 'main()' function will sleep for a period of time, and upon awakening will terminate the application.
* The 'main()' function will be passed three parameters on the command line:
    1. How long to sleep before terminating.
    2. The number of producer threads.
    3. The number of consumer threads.
* main() pseudocode:
    1. Get commad line args argv[1], argv[2], argv[3].
    2. Initialize buffer.
    3. Create producer thread(s).
    4. Create consumer thread(s).
    5. Sleep.
    6. Exit.
* The producer thread(s) will alternate between sleeping for a random period of time and inserting a random integer into the buffer. Random numbers will be produced using the 'rand()' function, which produces random integers. The consumer thread(s) will also sleep for a random period of time, and upon awakening will attempt to remove an item from the buffer. 
* Print statements related to produced and consumed items are printed as each producer and consumer completes an action. 
* Additionally, deffered cancellation is implemented for the producer and consumer threads through a signal handler overriding Ctrl-c (SIGINT), which sets a global variable telling the threads to exit. Making it so instead of waiting for the entire sleep time before terminating the threads, the threads can be terminated in a deffered way at any time during runtime by entering Ctrl-c. The deffered thread cancellation is done through the use of 'pthread_cancel()' and 'pthread_join()' functions.
* All pthread/semaphore/command line operations have error checks implemented to properly handle any major errors. Upon an error, a descriptive error message will be printed to stderr.
## Screenshot
![Alt text](/screenshot/sc.png?raw=true "sc")
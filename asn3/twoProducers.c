/***********************************************************
 * Assignment 3 Part 4 - Two Producers                     *
 * Michelle Stakenborg                                     *
 * Course: CS3305                                          *
 *                                                         *
 * This program creates two buffers of given size  and     *
 * reproduces the consumer-producer problem with given     *
 * number of items. Prints statements regarding p/c actions*
 *                                                         *
 * ********************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <semaphore.h>

/*global variables*/
int finished = 0;  //indicates when all items have been put into buffers 

/*declare semaphore and global variable*/
sem_t bsems[2]; //sems to control stall/release on buffer full (cons pops)
sem_t csems[2]; //sems to control stall/release on buffer empty (prods push)

/*stack structure for buffers*/
typedef struct Stack
{
  int name;       //buffer name 1/2 
  int capacity;   //size of buffer
  int size;       //current number of elements in buffer
  int elements;   //number of elements producer will produce
  int *items;     //array to hold items pushed to buffer
}Stack;

/*declare stack methods*/
Stack * createStack(int n, int cap, int size, int numElem); 
int pop(Stack *S); 
void push(Stack *S, int element); 
int isEmpty(Stack *S); 
int isFull(Stack *S); 

/*stack declarations*/
Stack *ProdA, *ProdB; 

/*consume method takes items from two buffers until both buffers are empty
 * and all the items entered have been processed then exits the program*/
void *consume(void *arg)
{
  while(1)
  {
    /*if both producers are finished and both buffers are empty, exit*/
    if(finished = 2 && isEmpty(ProdA) && isEmpty(ProdB))
    {
      printf("Finished executing all items. Exiting program...\n"); 
      exit(0); 
    }
 
    /*pull items from buffer A*/
    while(isEmpty(ProdA) != 1)
    {
       pop(ProdA);
       printf("Consumer has consumed an item from producer %d\n", ProdA->name); 
       sem_post(&bsems[0]);
       if(isEmpty(ProdA))
       {
         sem_wait(&csems[0]);
       }
    }
    while(isEmpty(ProdB) != 1)
    {
       pop(ProdB); 
       printf("Consumer has consumed an item from producer %d\n", ProdB->name); 
       sem_post(&bsems[1]); 
       if(isEmpty(ProdB))
       {
         sem_wait(&csems[1]);
       }
    }
  }
}

/*produce method puts number of items specified in argument into buffers.
 * Stalls if buffer is full and signals consumer when an item is pushed*/
void *produce(void *arg)
{
  Stack *buffer = (Stack *)arg;    //get buffer details
  int numItems = buffer->elements; //get number of items to produce
  int bufName = buffer->name; 
  int i;
  for(i=0; i < numItems; i++)
  {
      if(bufName == 1)
      {
        //if the buffer is full, wait until consumer takes item
        if(isFull(ProdA))
        {
          sem_wait(&bsems[0]); 
        }
         push(ProdA, i); 
         printf("Producer %d has put an item in its buffer\n", bufName);
         sem_post(&csems[0]); 
       }
        
      if(bufName == 2)
      {
        if(isFull(ProdB))
        {
          sem_wait(&bsems[1]);
        }
        push(ProdB, i);
        printf("Producer %d has put an item in its bufferr\n", bufName);
        sem_post(&csems[1]);
      }
  } 
  finished++;
}

/* main program creates the threads for two producers and one consumer. 
 * Then redirects threads to specified methods 
 * Cleans up after return */
int main(int argc, char** argv) 
{ 
  int bufferAsize, bufferBsize, bufferAitems, bufferBitems; 
  
  /*check for command line argument - assumes valid digit entered*/
  if (argc != 5){
      perror("Please enter buffer sizes and number of items\n");
      return 1;
  }

  /*get buffer sizes and item numbers from command line argument*/
  bufferAsize = atoi(argv[1]);
  bufferBsize = atoi(argv[2]);
  bufferAitems = atoi(argv[3]);
  bufferBitems = atoi(argv[4]); 

  /*initialize sempahores*/
  sem_init(&bsems[0], 0, 0);
  sem_init(&bsems[1], 0, 0);  
  sem_init(&csems[1], 0, 1); 
  sem_init(&csems[2], 0, 1); 

  /*create stacks*/ 
  ProdA = createStack(1, bufferAsize, 0, bufferAitems); 
  ProdB = createStack(2, bufferBsize, 0, bufferBitems); 

  /*create threads*/
  pthread_t producerA, producerB, consumer;


  /*create threads and execute print statements for number of iterations*/
    if (pthread_create(&consumer, NULL, consume, NULL))
    {
      fprintf(stderr, "Error creating consumer\n");
      exit(1);
    }
    if(pthread_create(&producerA, NULL, produce, (void *)ProdA))
    {
      fprintf(stderr, "Error creating producerA\n");
      exit(1); 
    }
    if (pthread_create(&producerB, NULL, produce, (void *)ProdB))
    {
      fprintf(stderr, "Error creating producerB\n"); 
      exit(1); 
    }
  

  pthread_join(consumer, NULL);
  pthread_join(producerA, NULL); 
  pthread_join(producerB, NULL); 
  

  /*cleanup*/
  sem_destroy(csems);
  sem_destroy(bsems); 

  pthread_exit(0); 
}

/*Stack methods create and manipulate stack options to create stacks
 * push items onto the stack, and pull items from the stack*/ 
Stack * createStack(int n, int cap, int size, int numElem)
{
  Stack *S;
  S = (Stack *)malloc(sizeof(Stack));
  S->name = n; 
  S->items = (int *)malloc(sizeof(int)*cap); 
  S->size = 0; 
  S->elements = numElem; 
  S->capacity = cap; 
  return S;
}

void push(Stack *S, int element) {
  if(S->size == S->capacity)
  {
      printf("Error stack full.\n");  //failsafe  
      return; 
  }
  else{
      S->items[S->size++] = element;
   }
}

int pop(Stack *S){
  if(S->size == 0)
  {
    printf("Error stack empty\n");  //failsafe
    return -1; 
  }
  else
  {
    int returnVal = S->items[S->size-1];
    S->size--;
    return returnVal; 
  }
}

int isEmpty(Stack *S){
    if(S->size == 0)
    {
      return 1; 
    }
    else
    {
      return 0; 
    }
}

int isFull(Stack *S)
{
  if(S->size == S->capacity)
  {
    return 1; 
  }
  else
  {
    return 0; 
  }
}

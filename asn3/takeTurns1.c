#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <semaphore.h>
#define NUMTHREAD 2

/*declare semaphore*/
sem_t semA, semB;
int counter; 
int iters; 
//int aTurn;
//int bTurn; 

void *printStmtA(void *arg)
{
  int i=0; 
  while(i < iters)
  {
      /*sem down*/
      sem_wait(&semB);
      /*CRITICAL SECTION*/
      char* thread = (char*)arg; 
      printf("Thread %s executing op%s\n", thread, thread);
      /*END CRITICAL SECTION*/
      sem_post(&semA);
      i++;
  }
}

void *printStmtB(void *arg)
{
  int i=0;
  while(i < iters)
  {
    /*sem down*/
    sem_wait(&semA);
    /*CRITICAL SECTION*/
    char* thread = (char*)arg;
    printf("thread %s executing op%s\n", thread, thread);
    /*END CRITICAL SECTION*/
    sem_post(&semB);
    i++;
  }
}

int main(int argc, char** argv) 
{

  /*pthreads A & B variables*/
  pthread_t threadA; 
  pthread_t threadB;
  sem_init(&semA, 0, 0); 
  sem_init(&semB, 0, 1);

  char *charA = "A";
  char *charB = "B";
  int i; 

  if (argc != 2){
      perror("Please enter number of iterations\n");
      return 1;
  }

  /*get number of iterations from command line argument*/
  iters = atoi(argv[1]);
  counter = iters*NUMTHREAD; 

  /*create threads and execute print statements for number of iterations*/
 // for(i = 0; i < iters; i++)
 // {
    if (pthread_create(&threadA, NULL, printStmtA, (void *)charA))
    {
      fprintf(stderr, "Error creating thread A\n");
      exit(1);
    }

    if (pthread_create(&threadB, NULL, printStmtB, (void *)charB))
    {
      fprintf(stderr, "Error creating thread B\n");
      exit(1);
    }
  //}

  /*join thread to avoid early exit and destroy sem after return*/
  pthread_join(threadA, NULL);
  pthread_join(threadB, NULL);

  sem_destroy(&semA);
  sem_destroy(&semB);
  pthread_exit(0); 
}


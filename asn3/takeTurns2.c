/***********************************************************
 * Assignment 3 Part 2 - Taking Turns 2                    *
 * Michelle Stakenborg                                     *
 * Course: CS3305                                          *
 *                                                         *
 * This program creates two threads A and B and            *
 * utilizes semaphores to allow the threads to take        *
 * turns executing a print statement                       *
 *                                                         *
 * ********************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <semaphore.h>
#define NUMTHREAD 2

/*declare semaphore and global variable*/
sem_t semA, semB;
int iters; 

/* operation A */
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

/* operation B */
void *printStmtB(void *arg)
{
  int i=0;
  while(i < iters)
  {
    /*sem down*/
    sem_wait(&semA);

    /*CRITICAL SECTION*/
    char* thread = (char*)arg;
    printf("Thread %s executing op%s\n", thread, thread);
    /*END CRITICAL SECTION*/
    
    sem_post(&semB);
    i++;
  }
}

/* main program creates the threads and cleans up after return */
int main(int argc, char** argv) 
{

  /*pthreads A & B variables*/
  pthread_t threadA; 
  pthread_t threadB;

  /*initialize semaphores*/
  sem_init(&semA, 0, 1); 
  sem_init(&semB, 0, 1);

  /*variables to pass to print statement - Allows expansion*/
  char *charA = "A";
  char *charB = "B";
  int i; 

  /*check for command line argument - assumes valid digit entered*/
  if (argc != 2){
      perror("Please enter number of iterations\n");
      return 1;
  }

  /*get number of iterations from command line argument*/
  iters = atoi(argv[1]);

  /*create threads and execute print statements for number of iterations*/
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
 

  /*join thread to avoid early exit and destroy sem after return*/
  pthread_join(threadA, NULL);
  pthread_join(threadB, NULL);

  /*cleanup*/
  sem_destroy(&semA);
  sem_destroy(&semB);
  pthread_exit(0); 
}


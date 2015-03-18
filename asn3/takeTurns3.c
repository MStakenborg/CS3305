/***********************************************************
 * Assignment 3 Part 3 - Taking Turns 3                    *
 * Michelle Stakenborg                                     *
 * Course: CS3305                                          *
 *                                                         *
 * This program creates a given number of threads and      *
 * utilizes semaphores to allow the threads to take        *
 * turns executing a print statement                       *
 *                                                         *
 * ********************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <semaphore.h>


/*declare semaphore and global variable*/
sem_t *sems;
int iters = 0; 
int nthreads = 0; 

void *printStmt(void *id)
{
  int i; 
  int thread = (int)id; 
  for(i=0; i < iters; i++)
  {
    sem_wait(&sems[thread]); 
    printf("Thread %d executing op%d\n", thread, thread); 
    if(thread+1 > nthreads)
    {
      sem_post(&sems[0]); 
    }
    else
    {
      sem_post(&sems[thread+1]);
    }
  }
}


/* main program creates the threads and cleans up after return */
int main(int argc, char** argv) 
{ 
  int i, j, t; 
  
  /*check for command line argument - assumes valid digit entered*/
  if (argc != 3){
      perror("Please enter number of iterations\n");
      return 1;
  }

  /*get number of iterations and threads from command line argument*/
  iters = atoi(argv[1]);
  nthreads = atoi(argv[2]);

  /*create threads*/
  pthread_t threads[nthreads];

  sems = malloc(nthreads * sizeof(sems[0]));  

  sem_init(&sems[0], 0, 1); 

  int k;
  for(k=1; k < iters; k++)
  {
    sem_init(&sems[k], 0, 0);
  }

  /*create threads and execute print statements for number of iterations*/
  for(t=0; t < nthreads; t++)
  {
    int *arg = malloc(sizeof(*arg));
    *arg = t; 
  
    if (pthread_create(&threads[t], NULL, printStmt,(void *)t))
    {
      fprintf(stderr, "Error creating thread A\n");
      exit(1);
    }
  }

  for(j=0; j < nthreads; j++)
  {
     pthread_join(threads[j], NULL);
  }
  

  /*cleanup*/
  sem_destroy(sems);
  pthread_exit(0); 
}


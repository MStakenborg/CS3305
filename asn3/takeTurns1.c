#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void *printStmt(void *arg)
{
  char* thread = (char*)arg; 
  printf("Thread %s executing op%s\n", thread, thread);
}

int main(int argc, char** argv) 
{
  int iters;

  /*pthreads A & B variables*/
  pthread_t threadA; 
  pthread_t threadB;

  char *charA = "A";
  char *charB = "B";
  int i; 

  if (argc != 2){
      perror("Please enter number of iterations\n");
      return 1;
  }

  /*get number of iterations from command line argument*/
  iters = atoi(argv[1]);

  /*create threads and execute print statements for number of iterations*/
  for(i = 0; i < iters; i++)
  {
    if (pthread_create(&threadA, NULL, printStmt, (void *)charA))
    {
      fprintf(stderr, "Error creating thread A\n");
      exit(1);
    }

    if (pthread_create(&threadB, NULL, printStmt, (void *)charB))
    {
      fprintf(stderr, "Error creating thread B\n");
      exit(1);
    }
  }

  pthread_exit(NULL); 
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <sched.h>
#define SEED 35791246

double computeElapsedTime(struct timeval start_time, struct timeval finish_time);

main(int argc, char **argv)
{
  int niter, nchild, policy;
  double x, y;
  int i, j,k, count = 0; /*# of points in the 1st quadrant of unit circle */
  double z;
  double pi;
  pid_t pid; 
  struct sched_param param; 
  struct timeval start, finish; 
  double elapsedTime; 

  if (argc != 3){
     perror("incorrect arguments");
     exit(0);
  }

  niter = atoi(argv[1]);
  nchild = atoi(argv[2]); 

  /*loop for the number of children*/
  for (j = 0; j < nchild; j++){
       /*set the schedule priority*/    
       k = j; 
       if(k % 2 == 0){
         policy = SCHED_OTHER;
         printf("Policy is sched other \n"); 
       }
       else{
         policy = SCHED_FIFO; 
         printf("Policy is sched fifo \n"); 
       }
       
       param.sched_priority = sched_get_priority_max(policy); 
      
       if(sched_setscheduler(0, policy, &param)){
         perror("Error setting scheduler policy");
         exit(EXIT_FAILURE);
       }
      
       pid = fork(); 
       if (pid < 0){
           perror("Error forking.");
           exit(pid); 
        }
        /*parent process */
        if (pid > 0){
           // exit(0);
        }
        if (pid == 0){
            /* initialize random numbers */
            srand(SEED);
            /*start timer*/
            gettimeofday(&start, NULL);

            count = 0;
            for (i = 0; i<niter; i++) {
                 x = (double)rand() / RAND_MAX;
                 y = (double)rand() / RAND_MAX;
                 z = x*x + y*y;
                 if (z <= 1) count++;
            }
            pi = (double)count / niter * 4;

            /*stop timer*/
            gettimeofday(&finish, NULL);
            /*compute elapsed time*/
            elapsedTime = computeElapsedTime(start, finish); 
            printf("Elapsed time is %f \n", elapsedTime); 
            printf("# of trials= %d , estimate of pi is %g \n", niter, pi);
            exit(0);
        }
     }
           
}


/*This function finds the difference between two times */
double computeElapsedTime(struct timeval start_time, struct timeval finish_time){
       double start_count, end_count, elapsed_time;
       start_count =
       (double)start_time.tv_sec + 1.e-6*(double)start_time.tv_usec;
       end_count =
       (double)finish_time.tv_sec + 1.e-6*(double)finish_time.tv_usec;
       elapsed_time = (end_count - start_count);
       return elapsed_time;
}


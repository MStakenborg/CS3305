/*************************************************************/
/* Simple shell                                               /
 * Author @ Michelle Stakenborg                               /
 * Course: CS3350 UWO                                         /
 *                                                            /
 * Description: This shell takes in commands and executes     /
 * Keeps a history and handles pipes and redirects            /
 *                                                            /
 *************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define MAX 256
#define CMD_MAX 30 

/*global variables*/
static const char *history[MAX];
static unsigned histCount = 0; 

int getValue(char *num)
{
  int value;
  value = atoi(num);
  if(value == 0)
  {
    printf("Please input a digit for history\n");
    return -1;
  }
  return value;
}

int printHistory(char *max)
{
  int value;
  if(histCount == 0)
  {
    /*no history to print yet*/
    printf("No commands have been entered."); 
  }
  else
  {
    int h;
    /*if no number specified - print commands up to 10*/
    if(max == NULL)
    {
      if(histCount < 10){
         for(h = 0; h < histCount; h++)
         {
            printf("%s\n", history[h]); 
         }
      }
      else{
        int temp = histCount;
        for(h=(temp - 10); h < 10; h++)
         {
           printf("%s\n", history[h]);
         }
      }
    }
    /*else print the specified number of commands up to command count*/
    else
    {
      value = getValue(max); 
     
      if(value == -1)
      {
        return (0);
      }
      if(value < histCount)
      {
        for(h = 0; h < value; h++)
        {
           printf("%s\n", history[h]); 
        }
      }
      else
      {
        for(h = 0; h < histCount; h++)
        {
          printf("%s\n", history[h]);
        }
      }
    }
  }
  return 0; 
}

void addHistory(const char *command)
{
   history[histCount] = command; 
   histCount++;
}


/*make tokenlist takes the following parameters..
 * buf input representing string for which the tokens are determined 
 * tokens: represents the array the tokens are put into
 * strtok() function: used to find the tokens distinguished by spaces */

int make_tokenlist(char *buf, char *tokens[])
{
  char input_line[MAX]; 
  char *line; 
  int i, n; 
  i = 0; 

  line = buf; 
  tokens[i] = strtok(line, " ");
  do  {
      i++;
      line = NULL; 
      tokens[i] = strtok(line, " ");
  } while (tokens[i] != NULL); 

  return i; 
}


/* main program uses the tokenizer to determine user input and redirects as necessary*/

void main(void)
{
    char input_line[MAX], *tokens[CMD_MAX], *history[CMD_MAX]; 
    char *execA[CMD_MAX], *execB[CMD_MAX], *execC[CMD_MAX], *execD[CMD_MAX];
    char final[CMD_MAX]; 
    int i,n,a,c,flag = 0, status = 2, count=0, piped = 0, redirin = 0, redirout = 0;
    pid_t pid, pid2;                             

    while(1)
    {
      flag = 0, piped = 0, redirin = 0, redirout = 0;
      printf("mstakenb> ");
      fgets(input_line, MAX, stdin);
      addHistory(input_line);

      /*prepare pipe & redirect matrices*/
      for(c = 0; c < CMD_MAX; c++){
        execA[c] = NULL; 
        execB[c] = NULL;
        execC[c] = NULL;
        execD[c] = NULL;
      }

       /*check for blank input*/
      if(!strcmp(input_line, "\n"))
      {
         printf("Invalid input. Try again...\n");
         flag = 1;
      }
     
      else{
        input_line[strlen(input_line) -1] = '\0';
              
        /*check for exit command*/
        if(!strcmp(input_line, "exit"))
        {
           exit(0);
        }
   

        /*parse command given*/
        n = make_tokenlist(input_line, tokens);
        
        
        if(!strncmp(tokens[0], "history", 6))
        {
          flag = 1;
          int result;
          if(n == 1)
          {
             result = printHistory(NULL); //print 10 commands by default
          }
          if(n > 1)
          {
             result = printHistory(tokens[1]);
          }
          if(result == -1)
          {
             printf("error printing history\n");
             exit(0);
          }
        }      

        /*check if command is a pipe command or redirect*/
        for(a = 0; a < n; a++){
           if(!strcmp(tokens[a], "|")){
                 piped++;  //pipe command entered
            }
            if(!strcmp(tokens[a], "<")){
                  redirin++;  //redireted in command
            }
            if(!strcmp(tokens[a], ">")){
                  redirout++; //redirected out command
            }
         }

        /*if exit, blank input or history was not entered execute cmd*/
        if(flag != 1){
        int fd[2];

        pid = fork();
    
        if(pid <  0)
        {
           perror("error forking");
           exit(pid); 
        }

        if(pid  > 0)
        {
           wait(NULL);
        }

        /*child process*/
        if(pid == 0)
        {
          /*if single commands with arguments - no pipe, no redirect*/
          if(piped == 0 && redirin == 0 && redirout == 0)
          {
            if(input_line != NULL)
            {
               /*execute given command(s) with/without args*/
               status = execvp(tokens[0], tokens);
               if(status == -1)
               {
                 printf("Error executing command, try again...\n");
                 exit(0);
               }
            }
          }
        
         /*pipe command selected*/ 
         else
         {
            pipe(fd);
            int retValue = 2; 

            /*if only one pipe or redirect-copy commands to 2 new arrays*/
            int b = 0, d=0;
            if(piped != 0)
            {
              while(strcmp(tokens[b], "|"))
              {
               execA[b] = tokens[b];
               b++;
               }
              if(piped > 1)
              {
                /*two pipes*/
                b++;
                int c=0; 
                while(strcmp(tokens[b], "|"))
                {
                  execC[c] = tokens[b];
                  b++;
                  c++;
                }
                /*three pipes*/
                if(piped > 2)
                {
                  b++; 
                  int d = 0;
                  while(strcmp(tokens[b], "|"))
                  {
                    execD[d] = tokens[b];
                    b++;
                    d++;
                  }
                }
                
              }
            }
            if(redirout != 0)
            {
               while(strcmp(tokens[b], ">"))
               {
                 execA[b] = tokens[b];
                 b++;
               }
            }
            else if(redirin != 0)
            {
              while(strcmp(tokens[b], "<"))
              {
                execA[b] = tokens[b];
                b++;
              }
            }
            b++; //account for pipe or redirect character
    
            /*fill in final array with final command*/
            while(tokens[b] != NULL)
            {
               execB[d] = tokens[b];
               b++; d++;
            }

            pid2 = fork();
               
            if(pid2 < 0) 
            {
               perror("Error forking \n");
               exit(-1);
            }

            /*Parent in pipe or redirect command*/
            if(pid2 > 0)
            {
               close(fd[0]); 
               if(dup2(fd[1], STDOUT_FILENO) < 0)
               {
                  perror("Error dup in parent\n");
                  exit(1); 
               }

               if(redirout > 0 || piped > 0)
               {
                 retValue = execvp(execA[0], execA);
               }

               if(redirin > 0)
               {
                 retValue = execvp(execB[0], execB);
               }
               if(retValue < 0)
               {
                  perror("Error - exec in parent\n");
                  exit(0);
               }
               wait(NULL);
               close(fd[1]);
            }

            /*Child in pipe/redirect command*/
            else
            {
              close(fd[1]);
              if(dup2(fd[0], STDIN_FILENO) < 0)
              {
                 perror("Error dup in child\n"); 
                 exit(1);
              }

              if(redirout == 1 || piped == 1)
              {
                retValue = execvp(execB[0], execB);
              }
              else if(redirin == 1)
              {
                retValue = execvp(execA[0], execA);
              }

              if(retValue < 0){
                perror("Cannot execute\n");
                exit(1);
              }

              /*two or more pipes/redir detected - fork new child*/
              if(piped > 1 || redirout > 1 || redirin > 1)
              {
                int fd2[2];
                pid_t pid3;
                int retVal2 = 2; 

                if(pipe(fd2) < 0)
                {
                  perror("Error piping in second iter\n"); 
                  exit(1);
                }
                
                pid3 = fork();

                if(pid3 < 0)
                {
                  printf("Error forking in second child\n"); 
                  exit(-1);
                }
                
                /*parent process two pipes*/
                if(pid3 > 0)
                {
                  close(fd2[0]); 
                  if(dup2(fd2[1], STDOUT_FILENO) < 0)
                  {
                    perror("Error in dup second iter\n"); 
                    exit(1); 
                  }

                  retVal2 = execvp(execC[0], execC);

                  if(retVal2 < 0)
                  {
                     perror("Error exec in second parent\n"); 
                     exit(1);
                  }
                  wait(NULL);
                  close(fd2[1]);
                }

                /*child process two pipes*/
                else
                {

                  close(fd2[1]);
                  if(dup2(fd2[0], STDIN_FILENO) < 0)
                  {
                    perror("Error in dup second iter child\n"); 
                    exit(1); 
                  }

                  if(piped == 2)
                  {
                    retVal2 = execvp(execB[0], execB);
                  }

                  if(retVal2 < 0)
                  {
                    perror("Error exec in second child\n"); 
                    exit(1); 
                  }

                  if(piped > 2)
                  {
                    int fd3[2];
                    pid_t pid4; 
                    int retVal3 = 2;

                    if(pipe(fd3) < 0)
                    {
                      perror("Error piping in third iter\n");
                      exit(1);
                    }

                    pid4 = fork();

                    if(pid4 < 0)
                    {
                      perror("Error piping in third iter\n");
                      exit(-1);
                    }

                    /*parent process- three pipes*/
                    if(pid4 > 0)
                    {
                      close(fd3[0]);
                      if(dup2(fd3[1], STDOUT_FILENO) < 0)
                      {
                        perror("Error in dup third iter]n");
                        exit(1);
                      }

                      retVal3 = execvp(execD[0], execD);

                      if(retVal3 < 0)
                      {
                        perror("Error exec in third parent\n");
                        exit(1);
                      }
                      wait(NULL);
                      close(fd3[1]);
                    }

                    /*child process three pipes*/
                    else
                    {
                      close(fd3[1]);
                      if(dup2(fd3[0], STDIN_FILENO) < 0)
                      {
                        perror("Error in dup third iter child\n");
                        exit(1);
                      }

                      retVal3 = execvp(execB[0], execB);

                      if(retVal3 < 0)
                      {
                        perror("Error exec in third child\n");
                        exit(1);
                      }
                      close(fd3[0]);
                    }
                    close(fd2[0]);
                  }
                 close(fd[0]);
                }
              }
            }
          }
        }
      }
     }
   }
}


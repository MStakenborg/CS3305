#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define MAX 256
#define CMD_MAX 10

/*global variables*/
static const char *history[CMD_MAX];
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
    if(max == NULL)
    {
       for(h = 0; h < histCount; h++)
       {
          printf("%s\n", history[h]); 
       }
    }
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
    char *execA[CMD_MAX], *execB[CMD_MAX];
    char final[CMD_MAX]; 
    int i,n,a, status = 2, count=0, piped = 0, redirin = 0, redirout = 0;
    pid_t pid, pid2;                             

    while(1)
    {
      printf("mstakenb> ");
      fgets(input_line, MAX, stdin);
      addHistory(input_line);

       input_line[strlen(input_line) -1] = '\0';
       /*parse command given*/
       n = make_tokenlist(input_line, tokens);

       /*check if command is a pipe command or redirect*/
       for(a = 0; a < n; a++){
           if(!strcmp(tokens[a], "|")){
               piped = 1;  //pipe command entered
           }
           if(!strcmp(tokens[a], "<")){
             redirin = 1;  //redireted in command
           }
           if(!strcmp(tokens[a], ">")){
             redirout = 1; //redirected out command
           }
       }
      
       
       /*check for blank input*/
      if(!strcmp(input_line, "\n"))
      {
         printf("Invalid input. Try again...\n");
      }
      
      /*check for exit command*/
      if(!strcmp(input_line, "exit"))
      {
          exit(0);
      }
    
      if(!strncmp(tokens[0], "history", 6))
      {
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

    else{
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

      /*if single commands with arguments - no pipe*/
      if(piped == 0)
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
      else{
        pipe(fd);
        int retValue = 2; 
        pid2 = fork();


        /*copy commands to 2 new arrays*/
        int b = 0;
        while(strcmp(tokens[b], "|")){
           execA[b] = tokens[b];
           b++;
        }
        b++; //account for pipe character
        execA[b] = "\0";
    
        while(tokens[b] != NULL){
           execB[b] = tokens[b];
           b++;
        } 

               
        if(pid2 < 0) 
        {
            perror("Error forking \n");
            exit(-1);
        }

        /*Parent in pipe command*/
        if(pid2 > 0)
        {
          close(fd[0]); 
          if(dup2(fd[1], STDOUT_FILENO) < 0)
          {
            perror("Error dup in parent\n");
            exit(0); 
          }

          retValue = execvp(execA[0], execA);
          
          if(retValue < 0)
          {
            perror("Error - exec in parent\n");
            exit(0);
          }
          wait(NULL);
        }

        /*Child in pipe command*/
        else
        {
          close(fd[1]);
          if(dup2(fd[0], STDIN_FILENO) < 0)
          {
            perror("Error dup in child\n"); 
            exit(0);
          }

          retValue = execvp(execB[0], execB);

          if(retValue < 0)
          {
            perror("Error executing in child\n"); 
            exit(0);
          }
        }
       }
      }
     }
   }
}


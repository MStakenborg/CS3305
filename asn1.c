#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
    char final[CMD_MAX]; 
    int i,n,a, status = 2, count=0, pipe = 0;
    pid_t pid, pid2; 
    input_line[strlen(input_line) -1] = '\0';


    while(1)
    {
      printf("mstakenb> ");
      fgets(input_line, MAX, stdin);
      addHistory(input_line);

       input_line[strlen(input_line) -1] = '\0';
       /*parse command given*/
       n = make_tokenlist(input_line, tokens);

       /*check if command is a pipe command*/
       for(a = 0; a < n; a++){
           if(!strcmp(tokens[a], "|")){
               pipe = 1;  //pipe command entered
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
      if(pipe == 0)
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

          exit(0);
      }
       }
     }
   }
}


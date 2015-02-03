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
  return value;
}

void printHistory(char *max)
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
      if(value > 10)
      {
        value = 10; 
        printf("Printing the max 10 commands");
      }
      for(h = 0; h <= value; h++)
      {
        printf("%s\n", history[h]); 
      }
     }
  }
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
    int i,n,status = 2, count=0;
    pid_t pid, pid2; 


    while(1)
    {
      printf("mstakenb> ");
      fgets(input_line, MAX, stdin);
      addHistory(input_line);

       /*parse command given*/
       n = make_tokenlist(input_line, tokens);

      if(!strcmp(input_line, "\n"))
      {
         printf("Invalid input. Try again...\n");
      }
      
      input_line[strlen(input_line) -1] = '\0';
       
      /*check for exit command or blank input*/
      if(!strcmp(input_line, "exit"))
      {
          exit(0);
      }
    
      if(!strcmp(input_line, "history"))
      {
        if(n == 1)
        {
          printHistory(NULL); //print 10 commands by default
        }
        if(n > 1)
        {
          printHistory(tokens[1]);
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

    if(pid == 0)
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
     }
   }
}


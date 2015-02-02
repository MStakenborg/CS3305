#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX 256
#define CMD_MAX 10

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
    char input_line[MAX], *tokens[CMD_MAX]; 
    char final[CMD_MAX]; 
    int i,n,count=0;
    pid_t pid; 
while(1){
    printf("mstakenb> "); 
    fgets(input_line, MAX, stdin); 

    /*check for exit command or blank input*/ 
    if(!strcmp(input_line, "exit\n"))
    {
       exit(0); 
    }

    if(!strcmp(input_line, "\n"))
    {
      printf("Invalid input. Exiting program...\n"); 
      exit(0); 
    }
    if(input_line != NULL)
    {
    
      //fork
      pid = fork();
    
      if(pid <  0)
      {
         perror("error forking");
      }
    
      if(pid  > 0)
      {
        wait(0);
      }

      if(pid == 0)
      {
         /*parse command given*/
         n = make_tokenlist(input_line, tokens); 
        for(i = 0; i < n; i++)
        printf("extracted token is %s\n", tokens[i]);
        execvp(tokens[0], tokens); 
      }
    }
    else
    {
       printf("Invalid input. Exiting program...\n"); 
    }
}
}

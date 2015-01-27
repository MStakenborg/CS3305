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
    int i,n;

    printf("mstakenb> "); 
    fgets(input_line, MAX, stdin); 

    if(!strcmp(input_line, "exit\n"))
    {
       exit(0); 
     }

    else{
      if(input_line != NULL)
    {
       n = make_tokenlist(input_line, tokens); 
    }
    else
    {
       printf("Invalid input. Exiting program... /n"); 
    }

    /*tester line REMOVE OR MODIFY*/ 
    for(i = 0; i < n; i++)
       printf("extracted token is %s\n", tokens[i]); 
    }

}

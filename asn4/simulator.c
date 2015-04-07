/***********************************************************
 * Assignment 4 - Page Table Simulator                     *
 * Michelle Stakenborg                                     *
 * Course: CS3305                                          *
 *                                                         *
 * This program simulates page table searches and returns  *
 * the number of faults encountered                        *
 *                                                         *
 * ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/*structures*/
typedef struct pageInfo
{
  int frameNumber; 
  time_t lastUsed; //change to time of day
  int useCount; 
} pageInfoEntry;

/*global variables*/
int faults = 0; 
struct timeval curTime; 

/* main program receives arguments and runs simulator depending on user
 * defined algorithm Least Recently Used or Least Frequently Used */
int main(int argc, char** argv) 
{
  int frames;  
  char *algorithm; 
  char *filename; 
  FILE *file; 
  pageInfoEntry *pageTable;  //page Table array 
  pageInfoEntry *tlb;       //ignore -not being used for this assignment 
  char *query; 
  int lfu = 0; 
  int lru = 0; 

  /*check for command line argument - assumes valid digit entered*/
  if (argc != 4){
      perror("Please enter number of frames, filename and algorithm\n");
      return 1;
  }

  /*get number of frames from command line argument*/
  frames = atoi(argv[1]);
  /*make sure number of frames given isn't 0*/
  if(frames == 0)
  {
    printf("Please enter a size greater than 0\n"); 
    exit(0); 
  }
  filename = argv[2]; 
  file = fopen(filename, "r"); 
  algorithm = argv[3]; 

  /*deterime algorithm type and set*/
  if(strncmp(algorithm, "lfu", 3) == 0 || strncmp(algorithm, "LFU", 3) == 0)
  {
     lfu = 1;  
  }
  else if(strncmp(algorithm, "lru", 3) == 0 || strncmp(algorithm, "LRU", 3) == 0)
  {
     lru = 1;  
 }


  /*create space for arrays*/
  pageTable = (pageInfoEntry*)malloc(frames * sizeof(pageInfoEntry)); 

  /*initialize all frames in page table to -1*/
  int i, j, k; 
  for(i =0; i < frames; i++)
  {
    pageTable[i].frameNumber = -1; 
    pageTable[i].lastUsed = 0; 
    pageTable[i].useCount = 0; 
  }

  /*get number of queries from file*/
  int lineCount=0; //number of lines in the file

  int ch; 
  while(!feof(file))
  {
     ch = fgetc(file);
     if(ch == '\n')
     {
       lineCount++;
     }
  }
  
  /*allocate space for tlb array - ignore not used for this assignment*/
  tlb = (pageInfoEntry*)malloc(lineCount * sizeof(pageInfoEntry)); 
 
  /*initialize page table array with entries from file*/
  int pt = 0, q; 
  rewind(file);
  fscanf(file, "%d", &q); 
  while(!feof(file))
  {
    tlb[pt].frameNumber = q;
    tlb[pt].useCount = 0; 
    tlb[pt].lastUsed = 0; 
    pt++;
    fscanf(file, "%d", &q); 
  }

  /*read elements in file and check if they are in table yet*/
  rewind(file);
  int hit; 

  fscanf(file, "%d", &q); 
  while(!feof(file))
  {
    hit = 0; 
    
    /*check if frame number in table matches*/
    for(j = 0; j < frames; j++)
    {
       if(pageTable[j].frameNumber == q)
       {
         hit = 1;
         break; 
       }
    }
    if(hit != 1)
    {
      faults++; 
      //check if there is an empty frame, if so put new value in
      for(i=0; i < frames; i++)
      {
        if(pageTable[i].useCount == 0)
        {
          pageTable[i].frameNumber = q;
          pageTable[i].useCount++; 
          gettimeofday(&curTime, NULL); 
          pageTable[i].lastUsed = curTime.tv_usec;
          hit = 1; 
          break;
        }
      }
      /*if no empty slot (hit is still false), replace value dependent
      on chosen algorithm*/ 
      if(hit != 1)
      {
        /*********** Least Recently Used *************/
        if(lru == 1)
        {
          //replace value which was used least recently
          int m, o = 0; 
          long double oldest = pageTable[0].lastUsed; 
          for(m=1; m < frames; m++)
          {
            if(pageTable[m].lastUsed < oldest)
            {
              oldest = pageTable[m].lastUsed;
              o = m; 
            }
          }
          pageTable[o].frameNumber = q; 
          pageTable[o].useCount = 1; 
          gettimeofday(&curTime, NULL);
          pageTable[o].lastUsed = curTime.tv_usec;
        }
        else if(lfu == 1)
        {
          /************ Least Frequently Used *******/
          //replace least frequently used value
         int n, lu=0; 
         int leastUsed = pageTable[0].useCount;
         for(n=1; n < frames; n++)
         {
            if(pageTable[n].useCount < leastUsed)
            {
                 leastUsed = pageTable[n].lastUsed;
                 lu = n;
            }
         }
         pageTable[lu].frameNumber = q;
         pageTable[lu].useCount = 1;
         gettimeofday(&curTime, NULL);
         pageTable[lu].lastUsed = curTime.tv_usec;
        }
        else
        {
          perror("Error Encountered\n"); 
        }
      }

    }
    fscanf(file, "%d", &q);
  }
          

  int o;
  for(o=0; o < frames; o++)
  {
    printf("pageTable at %d is %d\n", o, pageTable[o].frameNumber); 
  }
  printf("Number of page faults incurred was %d\n", faults); 

  /*cleanup*/
  close(file); 
  free(tlb); 
  free(pageTable); 
  return 0; 
}


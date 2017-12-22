//Katharina Schwab, 9.10.2017
//System Programmierung labor 1

#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/time.h>
#include<math.h>
#include <stdlib.h>
#include<errno.h>

int main()
{
  unsigned char checksum = 0;
  int nrOfReads = 0;
  unsigned int blocksize = 0;
  int i = 0;			//loop checksum calc
  int outerLoop = 0;
  int retRead = 0;  
  long sec = 0;
  long usec = 0;
  long time = 0;		
  int retLSeek = 0;
  struct timeval start = {0,0};
  struct timeval end = {0,0};
  
  int fd = open("lab.reference", O_RDONLY);
  if (fd < 3)
    perror("ERROR AT OPEN");
  
  //print table header
  printf(" blocksize \t all reads \t time \t    checksum\n");

  //outer loop for 20 calcs
  for(outerLoop = 0; outerLoop<=21; outerLoop++)
  {
    blocksize = pow(2,outerLoop);
    
    char buffer[blocksize];
  
    //start timer
    gettimeofday(&start, NULL);
    
    //read complete file
    while ( (retRead = read(fd, buffer, blocksize)) > 0)
    {
      nrOfReads++;
      if(retRead == -1)
	perror("ERROR AT READ");
      
      //calc checksum
      for (i = 0; i < retRead; i++)
      {
	checksum += buffer[i];
      }
      
    }
    //end timer
    gettimeofday(&end, NULL);
    
    //calc complete time
    sec = end.tv_sec - start.tv_sec;
    usec = end.tv_usec - start.tv_usec;
    if (usec < 0)
    { 
      sec --;
    }  
    time = (sec * 1000000) + abs(usec);
    
    printf("%10d %15d %10ld %14d\n", blocksize, nrOfReads, time, checksum);
    
    //reset read/write pointer to begin of file
    retLSeek= lseek(fd, 0, SEEK_SET);
    if(retLSeek < 0)
    {//printf("ERROR AT LSEEK");
      perror("ERROR AT LSEEK");
    }
    
    //reset all vars
    checksum = 0;
    sec = 0;
    usec = 0;
    start.tv_sec = 0;
    start.tv_usec = 0;
    end.tv_sec = 0;
    end.tv_usec = 0;
    retRead = 0;
    nrOfReads = 0;
  }
 
 close(fd);
 
 return 0; 
}
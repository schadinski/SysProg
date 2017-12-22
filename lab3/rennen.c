/* author Katharina Schwab
 * date 17.11.2017
 * System Programmierung Labor 3 Teilaufgabe 1
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

//child-function
int run(char, int);

//information for each child
struct child
{
  pid_t pid;
  char name;
  time_t endTime;
  int totalTime;
};

int main(int argc,char* argv[])
{
  int k = 0;
  int i = 0;
  int j = 0;
  char alphabet = 'A';				//for naming the childs
  time_t startTime;
  pid_t parentPid = 0;		
  pid_t childPid = 0;
  
  int givenSpeed = atoi(argv[2]);		//max Speed is given in m/sec
  if(givenSpeed > 20)				//max possible speed is 20 m/sec
  { 
    givenSpeed = 20;
  }
  else if( givenSpeed < 1)			//min possible speed is 1 m/sec		
  {
    givenSpeed = 1;
  }
  
  int nrOfChilds = atoi(argv[1]);		//how manx childs should run
  
  if(nrOfChilds > 10)				//max 10 childs
  { 
    nrOfChilds = 10;
  } 
  else if (nrOfChilds < 1)			//min 1 child
  {
    nrOfChilds = 1;
  }
  
  struct child allChilds[nrOfChilds];		//all childs 
  struct child finishedChilds[nrOfChilds];	//pick up all finished child processes 
  
  time(&startTime);				//getting starttime for all childs
  for (i = 0; i<nrOfChilds; i++)	
  {
    allChilds[i].pid = fork();			//create n childs
    allChilds[i].name = alphabet;		//naming the child
    
    //child code
    if(allChilds[i].pid == 0 )			
    {
      run(allChilds[i].name, givenSpeed);
      break;
    }//end child code
    
    //parent code
    else if(allChilds[i].pid != 0)
    {
   
    alphabet++;					//increment the name
    parentPid = getpid();
    }//end parent code
  }//end for
  
  //parent code
  if(getpid() == parentPid)
  {    
    //wait for all childs    
      j = 0;
      while ( j < nrOfChilds)
      {
	childPid = waitpid(-1, NULL, 0);	
	for ( k=0; k<nrOfChilds; k++)			//get the name and end time of finished child
	{						//and calculate the total time
	  if (allChilds[k].pid == childPid)
	  {
	    time(&allChilds[k].endTime);
	    allChilds[k].totalTime = (int) (allChilds[k].endTime - startTime);
	    finishedChilds[j] = allChilds[k];
	  }
	} 
	j++;
      }//end while
    
    //print table header
    printf("PLATZ RENNER    ZEIT(sek)\n");
    //print table
    for (j= 0; j < nrOfChilds; j++)
    {
      printf("%2d %4c \t%d\n", (j+1),finishedChilds[j].name, (finishedChilds[j].totalTime));
    }
  }//end parent code  
  return 0;
}


int run(char myName, int maxSpeed)
{
  int myWay = 0;				//run distance 
  int distance = 80;				//complete distance
  char outDistance[80] = "-";			//string for output
  char step[2] = "-";				//array of 2 because /0
  
  int uSec = 0;					//time to sleep
  int actSpeed = 0;				//speed in this loop
  
  //set beginning number of rand with an addition of actual time and the own pid
  srand ( ( time(NULL) + getpid() ) );
  
  while ( myWay < distance)
  {
    actSpeed = ( rand() % maxSpeed + 1);	//speed in this loop, between 1 and maxSpeed
    uSec = ( 1000000 * 10 /actSpeed);		// calculate time to sleep
    
    myWay++;					//do one step
    usleep(uSec);		
    printf("%s%c\n",outDistance ,myName);
    strcat(outDistance, step);
  }
  
  return 0;;
}
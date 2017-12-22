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
#include<errno.h>

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
  char speedBuffer[3];				//for converting given Speed to ASCII
  char *myArgv[4];				//need it at exec-call
  int execReturn = 0;
  char alphabet = 'A';				//for naming the childs
  char *name = &alphabet;			//get an char pointer to fill myArgv for exec call
  time_t startTime;
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
    
    //child code
    if(allChilds[i].pid == 0 )			
    {
      sprintf(speedBuffer,"%d",givenSpeed);	//convert givenSpeed(int) to char array
      myArgv[0] = "./renner";			//input values in myArgv
      myArgv[1] = name;			
      myArgv[2] = speedBuffer;			//can't use argv[2] because have to chack validation before use, see line 37-44
      myArgv[3] = NULL;
      execReturn = execvp("./renner",myArgv);	//call extern child code
      if(execReturn <0 )
      {
	perror("EXEC ERROR\n");
      }
      break;
    }//end child code
    
    //parent code
    else if(allChilds[i].pid != 0)
    {
   
    allChilds[i].name = alphabet;		//naming the child
    alphabet++;					//increment the name
    }//end parent code
  }//end for
  
  //parent code 
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
  //}//end parent code  
  return 0;
}



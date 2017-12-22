
/* author Katharina Schwab
 * date 17.11.2017
 * System Programmierung Labor 3 Teilaufgabe 2
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char** argv)
{
    int maxSpeed = atoi(argv[2]);			// get speed in int from ASCII
    char name[2];
    strncpy(name, argv[1], 1);			//get name of this child, use this to get correct string
    name[1] = '\0';				//put end ofstring sign in name to kill some sideeffects like random CR in the output at line 37
    int myWay = 0;				//run distance
    int distance = 80;				//complete distance
    char outDistance[80] = "-";			//string for output
    char step[2] = "-";				//array of 2 because /0

    int uSec = 0;					//time to sleep
    int actSpeed = 0;				//speed in this loop

    //set beginning number of rand with an addition of actual time and the own pid
    srand ( ( time(NULL) + getpid() ) );

    while ( myWay < distance) {
        actSpeed = ( rand() % maxSpeed + 1);	//speed in this loop, between 1 and maxSpeed
        uSec = ( 1000000 * 10 / actSpeed);		// calculate time to sleep

        myWay++;					//do one step
        usleep(uSec);
        printf("%s%s\n", outDistance , name);
        strcat(outDistance, step);
    }

    return 0;
};

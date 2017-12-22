
//Katharina Schwab, 23.10.2017
//System Programmierung Labor 2

#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/time.h>
#include<math.h>
#include <stdlib.h>
#include<errno.h>
#include<string.h>

ssize_t read_buffered (int fd, void *buf, size_t nbytes);


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
    struct timeval start = {0, 0};
    struct timeval end = {0, 0};

    int fd = open("lab.reference", O_RDONLY);
    if (fd < 0) {
        perror("ERROR AT OPEN");
        exit(0);
    }


    //print table header
    printf(" blocksize \t all reads \t time \t    checksum\n");

    //outer loop for 20 calcs
    for(outerLoop = 0; outerLoop <= 21; outerLoop++) {
        blocksize = pow(2, outerLoop);

        char buffer[blocksize];

        //start timer
        gettimeofday(&start, NULL);

        /*printf("fd is %d\n", fd);
        sleep(3);*/
        //read complete file
        while ( (retRead = read_buffered(fd, buffer, blocksize)) > 0) {
            nrOfReads++;

            //calc checksum
            for (i = 0; i < retRead; i++) {
                checksum += buffer[i];
            }

        }
        //end timer
        gettimeofday(&end, NULL);

        //calc complete time
        sec = end.tv_sec - start.tv_sec;
        usec = end.tv_usec - start.tv_usec;
        if (usec < 0) {
            sec --;
        }
        time = (sec * 1000000) + abs(usec);

        printf("%10d %15d %10ld %14d\n", blocksize, nrOfReads, time, checksum);

        //reset read/write pointer to begin of file
        retLSeek = lseek(fd, 0, SEEK_SET);
        if(retLSeek < 0) {
            //printf("ERROR AT LSEEK");
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


ssize_t read_buffered (int fd, void *buf, size_t nbytes)
{
    //printf("start read_buffered\n");
    static char sysbuf[5000];	//funktionsinterner Puffer
    static int maxForIoPointer = 5000;//Anzahl der maximal möglich lesbaren Bytes;
    static void* ioPointer = &sysbuf[0];//setzte Lese/SchreibePointer auf Anfang von sysbuf
    static int counter = 0;
    static int bytesInSysbuf = 0;	//Anzahl der zuletzt aus dem Kernel gelesener Bytes
    int bytes_copied = 0;		//Anzahl Bytes, die im aktuellen call nach buf kopiert wurden -> zurückgegeben wurden; auch Lese/Schreibezeiger
    int bytesToRead = 0;
    int exit = 0;
//char* myBuf = (char*) buf;

    while (bytes_copied < nbytes) {	//&& counter < maxForIoPointer	&& exit == 0
        //printf("start while bytesInSysbuf is %d\n", bytesInSysbuf);

        //ist sysbuf leer?
        if(bytesInSysbuf == 0 || nbytes < bytesInSysbuf)		//if(bytes_copied == maxForIoPointer)
            //if nbytes<bytesInSysbuf
        {
            //Nachladen aus Kernel
            bytesInSysbuf = read(fd, sysbuf, 5000);
            ioPointer = &sysbuf[0];		//setze Lese/Schreibepointer auf den Anfand des funktionsinternen buffers

            if(bytesInSysbuf == -1)
                perror("ERROR AT READ");

            printf("Read called\n");

            //sollten weniger als 5000 Bytes gelesen worden sein, ist sysbuf nicht max. befüllt
            //implizit EOF
            if(bytesInSysbuf < 5000) {
                maxForIoPointer = bytesInSysbuf;
                exit = 1;
            }//endif

        }//endif

        //es können nur nbytes auf einmal kopiert werden,wenn mehr als nbytes noch in sysbuf sind
        if(nbytes <= bytesInSysbuf) {
            bytesToRead = nbytes;
        }
        //es sind weniger als nbytes in sysbuf verfügbar, nur der Rest(bytesInSysbuf) kann kopiert werden
        else {
            bytesToRead = bytesInSysbuf;
        }

        /*if(counter+=bytesToRead > maxForIoPointer)
        {
          printf("hier ist der Fehler\n");
        }*/
        //kopiere von sysbuf nach buf
        memcpy(&buf[bytes_copied], ioPointer, bytesToRead);
        bytes_copied += bytesToRead;
        ioPointer += bytesToRead;	//setze Lese/Schreibezeiger weiter
        counter += bytesToRead;
        bytesInSysbuf -= bytesToRead;	//reduziere Füllstand um zurückgegebene Bytes

        if(exit == 1) {
            return bytes_copied;
        }

    }//end while
    //printf("bytes_copied is %d, read_buffered end\n",bytes_copied);
    return bytes_copied;
}//end read_buffered

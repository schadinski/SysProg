
//Katharina Schwab
//System Programmieriung Labor 2

#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/time.h>
#include<math.h>
#include <stdlib.h>
#include<errno.h>
#include<string.h>


ssize_t read_buffered (int fd, void *buf, size_t nbytes)
{
    //printf("start read_buffered\n");
    static char sysbuf[5000];		//funktionsinterner Puffer
    static void* ioPointer = &sysbuf[0];	//setzte Lese/SchreibePointer auf Anfang von sysbuf
    static int bytesInSysbuf = 0;		//Anzahl der zuletzt aus dem Kernel gelesener Bytes
    int bytes_copied = 0;			//Anzahl Bytes, die im aktuellen call nach buf kopiert wurden -> zurückgegeben wurden; auch Lese/Schreibezeiger
    int bytesToRead = 0;			//Anzahl der aktuell zu kopierenden Bytes
    char* myBuf = (char*) buf;		//cast da buf void* ist

//ist sysbuf leer?
    if(bytesInSysbuf <= 0) {
        //Nachladen aus Kernel
        bytesInSysbuf = read(fd, sysbuf, 5000);
        ioPointer = &sysbuf[0];

        if(bytesInSysbuf == -1) {
            perror("ERROR AT READ");
            return -1;
        }
    }

//bis alle geforderten Bytes kopiert sind oder EOF erreicht
    while (bytes_copied < (nbytes - bytes_copied) && bytesInSysbuf != -1) {
        //nbytes bzw. genügend Bytes um Anforderung zu erfüllen vorhanden
        if( (nbytes - bytes_copied) <= bytesInSysbuf) {
            bytesToRead = (nbytes - bytes_copied);
            memcpy(&myBuf[bytes_copied], ioPointer, bytesToRead);
            bytes_copied += bytesToRead;
            ioPointer += bytesToRead;	//setze Lese/Schreibezeiger weiter
            bytesInSysbuf -= bytesToRead;	//reduziere Füllstand um zurückgegebene Bytes
        }
        //weniger als nbytes vorhanden
        else {
            bytesToRead = bytesInSysbuf;

            //kopiere Rest aus Sysbuf
            memcpy(&myBuf[bytes_copied], ioPointer, bytesToRead);
            bytes_copied += bytesToRead;
            ioPointer += bytesToRead;	//setze Lese/Schreibezeiger weiter
            bytesInSysbuf -= bytesToRead;	//reduziere Füllstand um zurückgegebene Bytes

            //nachladen aus Kernel
            bytesInSysbuf = read(fd, sysbuf, 5000);
            ioPointer = &sysbuf[0];		//setze Lese/Schreibepointer auf den Anfand des funktionsinternen Buffers
            if(bytesInSysbuf == -1) {
                perror("ERROR AT READ");
                return -1;
            }

            //genügend Bytes nachgeladen um nbytes, bzw geforderte Bytes voll zu machen
            if( (nbytes - bytes_copied) <= bytesInSysbuf) {
                //kopiere die noch fehlenden Bytes bis nbytes erreicht
                bytesToRead = (nbytes - bytes_copied);
                memcpy(&myBuf[bytes_copied], ioPointer, bytesToRead);
                bytes_copied += bytesToRead;
                ioPointer += bytesToRead;	//setze Lese/Schreibezeiger weiter
                bytesInSysbuf -= bytesToRead;	//reduziere Füllstand um zurückgegebene Bytes
            }
            //nicht genügend Bytes nachgeladen, und EOF
            else if (bytesInSysbuf < 5000) {
                //kopiere restlichen vorhandenen Bytes
                bytesToRead = bytesInSysbuf;
                memcpy(&myBuf[bytes_copied], ioPointer, bytesToRead);
                bytes_copied += bytesToRead;
                bytesInSysbuf = -1;		//markiere EOF, while wird beendet
            }
            //nichtgenügend Bytes um nbytes zu befriedigen, aber noch nicht EOF
            else {
                //kopiere alle
                bytesToRead = bytesInSysbuf;
                memcpy(&myBuf[bytes_copied], ioPointer, bytesToRead);
                bytes_copied += bytesToRead;
                ioPointer += bytesToRead;	//setze Lese/Schreibezeiger weiter
                bytesInSysbuf -= bytesToRead;	//reduziere Füllstand um zurückgegebene Bytes
            }
        }
    }//end while

    buf = (void*) myBuf;
    return bytes_copied;
}//end read_buffered
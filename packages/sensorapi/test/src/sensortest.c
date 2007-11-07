#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for usleep
#include <time.h>
#include <sys/time.h>

#include "sensorapi.h"




/*
 Depth
// Temp


 */
int main(int argc, char ** argv)
{
    printf("\nHello World\n");

    int stv=0, ftv=0;
    int fd = openSensorBoard("/dev/sensor");
    printf("\nSyncBoard says: %d", syncBoard(fd));
    printf("\n");

    struct timeval tv;
    gettimeofday(&tv, NULL);


    int ret = 0, err=0, i=0;
/*
    for(i=0; i<1000; i++)
    {
        printf("Depth: %d\n", readDepth(fd));
    }
    */

    long tst = 0;

    for(i=0; i<1000; i++)
    {
        //printf("MC result = %d\n", ret);

        ret = setSpeeds(fd, i/3,i/3,i/3,i/3);

        ret = readDepth(fd);
        if(ret == SB_ERROR)
            err++;

        printf("Depth: %d\n", ret);


        unsigned char temp[NUM_TEMP_SENSORS];
        ret = readTemp(fd, temp);

        if(ret == SB_ERROR)
            err++;

        ret = readStatus(fd);
        if(ret == SB_ERROR)
            err++;

        ret = readThrusterState(fd);
        if(ret == SB_ERROR)
            err++;

        gettimeofday(&tv, NULL);
        stv = tv.tv_usec;
        usleep(6 * 1000);       /* Nanosleep is equally as bad as usleep :( */
        gettimeofday(&tv, NULL);

        ftv = tv.tv_usec;
        if(ftv < stv)
            ftv += 1048576;

        tst += (ftv - stv); /* Calculate what the idle time delay actually was */

        ret = readSpeedResponses(fd);
        if(ret != 0)
            err++;
    }

    usleep(15 * 1000);
    ret = setSpeeds(fd, i, i, i, i); //0,0,0,0);
    printf("Speed result = %d\n", ret);
    usleep(15 * 1000);

    printf("Error count: %d\n", err);
    printf("Average downtime delay: %ld usec\n", tst/1000);
    return 0;
}


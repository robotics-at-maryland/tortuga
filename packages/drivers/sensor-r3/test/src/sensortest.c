#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for usleep
#include <time.h>
#include <sys/time.h>
#include "sensorapi.h"


int main(int argc, char ** argv)
{
    unsigned char temp[NUM_TEMP_SENSORS];
    int stv=0, ftv=0, fd=0;
    int ret = 0, err=0, i=0;
    long tst = 0;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    fd = openSensorBoard("/dev/sensor");

    if(fd == -1)
    {
        printf("Connection error\n");
        return -1;
    }

    printf("\nSyncBoard says: %d", syncBoard(fd));
    printf("\n");


    for(i=0; i<500; i++)
    {
        ret = setSpeeds(fd, i/3,i/3,i/3,i/3,i/3,i/3);
        if(ret == SB_ERROR)
            err++;

        ret = readDepth(fd);
        if(ret == SB_ERROR)
            err++;

        printf("Depth: %d\n", ret);

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
    ret = setSpeeds(fd, 0,0,0,0,0,0);
    printf("Last speed result = %d\n", ret);
    printf("Error count: %d\n", err);
    printf("Average downtime delay: %ld usec\n", tst/1000);
    return 0;
}


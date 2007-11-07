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

    int fd = openSensorBoard("/dev/ttyUSB0");
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

    for(i=0; i<1000; i++)
    {
        //printf("MC result = %d\n", ret);

        ret = setSpeeds(fd, i/3,i/3,i/3,i/3);

        ret = readDepth(fd);
        printf("Depth: %d\n", ret);

//        usleep(15 * 1000);
    //    printf("Speed result = %d\n", ret);

        gettimeofday(&tv, NULL);
        int stv = tv.tv_usec;

        usleep(12 * 1000);

        gettimeofday(&tv, NULL);
//         printf("Actually slept for %ld usec\n", tv.tv_usec - stv);


       // usleep(8500);
        ret = readSpeedResponses(fd);
        if(ret != 0)
            err++;
    }

    usleep(15 * 1000);
    ret = setSpeeds(fd, 0,0,0,0);
    printf("Speed result = %d\n", ret);
    usleep(15 * 1000);

    printf("Error count: %d\n", err);
    return 0;
}


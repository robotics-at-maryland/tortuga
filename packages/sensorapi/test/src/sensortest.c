#include <stdio.h>
#include <stdlib.h>
#include "sensorapi.h"

int main(int argc, char ** argv)
{
    printf("\nHello World\n");

    int fd = openSensorBoard("/dev/sensor");
    printf("\nSyncBoard says: %d", syncBoard(fd));
    printf("\n");

    int ret = 0, err=0, i=0;

    for(i=0; i<500; i++)
    {

        ret = setSpeeds(fd, 0,i,0,0);
        printf("Speed result = %d\n", ret);
        usleep(15 * 1000);

        ret = readSpeedResponses(fd);
        if(ret != 0)
            err++;
        printf("MC result = %d\n", ret);
    }

    usleep(15 * 1000);
    ret = setSpeeds(fd, 0,0,0,0);
    printf("Speed result = %d\n", ret);
    usleep(15 * 1000);

    printf("Error count: %d\n", err);
    return 0;
}


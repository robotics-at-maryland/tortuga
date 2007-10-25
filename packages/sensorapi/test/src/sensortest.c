#include <stdio.h>
#include <stdlib.h>
#include "sensorapi.h"

int main(int argc, char ** argv)
{
    printf("\nHello World\n");

    int fd = openSensorBoard("/dev/sensor");
    printf("\nSyncBoard says: %d", syncBoard(fd));
    printf("\n");

    int ret = 0;

    ret = setSpeeds(fd, 0,0,0,0);

    printf("Speed result = %d\n", ret);

    return 0;
}


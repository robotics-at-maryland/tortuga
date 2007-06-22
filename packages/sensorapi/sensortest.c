#include <stdio.h>
#include <stdlib.h>
#include "sensorapi.h"

int main(int argc, char ** argv)
{
    printf("\nHello World\n");

    int fd = openSensorBoard("/dev/ttyUSB0");

    unsigned char buf[3];
    buf[0]=0x0A;
    buf[1]=0x0A;

    write(fd, &buf, 2);

    printf("\nSyncBoard says: %d\n", syncBoard(fd));

    return 0;

}


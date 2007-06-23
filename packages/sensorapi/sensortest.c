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

    printf("\nSyncBoard says: %d", syncBoard(fd));
    printf("\nPing says: %d", pingBoard(fd));
    printf("\nCheck says: %d", checkBoard(fd));
    printf("\nDepth says: %d", readDepth(fd));
    printf("\nStatus says: %d", readStatus(fd));
    printf("\nHard Kill says: %d", hardKill(fd));

    printf("\nDrop Marker 0: %d", dropMarker(fd, 0));
    printf("\nDrop Marker 1: %d", dropMarker(fd, 1));



    printf("\n");
    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include "sensorapi.h"

int main(int argc, char ** argv)
{
    printf("\nHello World\n");

    int fd = openSensorBoard("/dev/ttyUSB0");
    printf("\nSyncBoard says: %d", syncBoard(fd));
    //printf("\nThruster safety 0: %d", thrusterSafety(fd, CMD_THRUSTER1_OFF));

    printf("\nHard Kill says: %d", hardKill(fd));
/*
    unsigned char buf[3];
    buf[0]=0x06;
    buf[1]=0x0A;

    write(fd, &buf, 1);
    printf("\nSyncBoard says: %d", syncBoard(fd));
    printf("\nPing says: %d", pingBoard(fd));
    printf("\nCheck says: %d", checkBoard(fd));
    printf("\nDepth says: %d", readDepth(fd));
    printf("\nStatus says: %d", readStatus(fd));
    printf("\nHard Kill says: %d", hardKill(fd));

    printf("\nDrop Marker 0: %d", dropMarker(fd, 0));
    printf("\nDrop Marker 1: %d", dropMarker(fd, 1));

    printf("\nLCD Backlight Off: %d", lcdBacklight(fd, 1));

    printf("\nThruster safety 0: %d", thrusterSafety(fd, 0));
    printf("\nThruster safety 1: %d", thrusterSafety(fd, 1));
    printf("\nThruster safety 2: %d", thrusterSafety(fd, 2));
    printf("\nThruster safety 3: %d", thrusterSafety(fd, 3));

    unsigned char tempData[6];

    printf("\nTemperature: %d", readTemp(fd, tempData));


    write(fd, &buf, 1);

    printf("\nBad sync text returned: %d",    dropMarker(fd, 0)); //displayText(fd, 0, "Angry Midget"));
    fflush(stdout);
    int i=0;



    printf("\n");
    return 0;
    while(1)
    {
    	int ret=0;
	    unsigned char bah[32];
	    sprintf(bah, "%d  ", i);
            ret= displayText(fd, 1, bah);

	    if(ret == SB_ERROR)
	    {
		    close(fd);
		    fd = openSensorBoard("/dev/sensor");
	    }
	    sleep(1);
	    i++;
    }
*/

    printf("\n");
    return 0;
}


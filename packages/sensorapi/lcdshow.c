#include <stdio.h>
#include <stdlib.h>
#include "include/sensorapi.h"

/*
 * Sensor Board Utility, Revision $rev$
 *
 */


int main(int argc, char ** argv)
{
    if(argc < 2)
    {
        printf("lcdshow -t line1 line2\n");
        printf("lcdshow -c  (clear screen)\n");
        printf("lcdshow -bloff (backlight on)\n");
        printf("lcdshow -blon  (backlight off)\n");
        printf("lcdshow -blfl  (backlight flash)\n");
        printf("lcdshow -diagon  (runtime diagnostics on)\n");
        printf("lcdshow -safe (disable thrusters)\n");
        printf("lcdshow -unsafe (enable thrusters)\n");
        printf("lcdshow -diagoff (runtime diagnostics off)\n");
	printf("lcdshow -s  (begin start sequence)\n");

	    return -1;
    }

    int fd = openSensorBoard("/dev/sensor");


    if(fd == -1)
    {
        printf("\nCould not find device!\n");
        close(fd);
        return -1;
    }

    if(syncBoard(fd) != 0)
    {
        printf("\nCould not sync with board!\n");
        close(fd);
    }

    if(strcmp(argv[1], "-safe") == 0)
    {
        if(thrusterSafety(fd, CMD_THRUSTER1_OFF) != SB_OK)
            printf("Error safing thruster\n");

        if(thrusterSafety(fd, CMD_THRUSTER2_OFF) != SB_OK)
            printf("Error safing thruster\n");

        if(thrusterSafety(fd, CMD_THRUSTER3_OFF) != SB_OK)
            printf("Error safing thruster\n");

        if(thrusterSafety(fd, CMD_THRUSTER4_OFF) != SB_OK)
            printf("Error safing thruster\n");

        close(fd);
        return 0;
    }

    if(strcmp(argv[1], "-unsafe") == 0)
    {
        if(thrusterSafety(fd, CMD_THRUSTER1_ON) != SB_OK)
            printf("Error unsafing thruster\n");

        if(thrusterSafety(fd, CMD_THRUSTER2_ON) != SB_OK)
            printf("Error unsafing thruster\n");

        if(thrusterSafety(fd, CMD_THRUSTER3_ON) != SB_OK)
            printf("Error unsafing thruster\n");

        if(thrusterSafety(fd, CMD_THRUSTER4_ON) != SB_OK)
            printf("Error unsafing thruster\n");
        close(fd);
        return 0;
    }

    if(strcmp(argv[1], "-diagon") == 0)
    {
        setDiagnostics(fd, 1);
        close(fd);
        return 0;
    }

    if(strcmp(argv[1], "-diagoff") == 0)
    {
        setDiagnostics(fd, 0);
        close(fd);
        return 0;
    }


    if(strcmp(argv[1], "-bloff") == 0)
    {
        lcdBacklight(fd, LCD_BL_OFF);
        close(fd);
        return 0;
    }

    if(strcmp(argv[1], "-blon") == 0)
    {
        lcdBacklight(fd, LCD_BL_ON);
        close(fd);
        return 0;
    }

    if(strcmp(argv[1], "-blfl") == 0)
    {
        lcdBacklight(fd, LCD_BL_FLASH);
        close(fd);
        return 0;
    }

    if(strcmp(argv[1], "-c") == 0)
    {
        displayText(fd, 0, "");
        displayText(fd, 1, "");
        close(fd);
        return 0;
    }



    if(strcmp(argv[1], "-s") == 0)
    {
    	displayText(fd, 1, "");
	    displayText(fd, 0, "Ready to start");
	    while((readStatus(fd) & STATUS_STARTSW) == 0);
	    displayText(fd, 0, "Running...");
	    close(fd);
	    return 0;
    }


    if(strcmp(argv[1], "-t") == 0)
    {
        displayText(fd, 0, argv[2]);

        if(argc == 4)
            displayText(fd, 1, argv[3]);
    }

    close(fd);
    return 0;
}


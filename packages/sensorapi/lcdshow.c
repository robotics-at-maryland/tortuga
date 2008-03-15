
// STD Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Project Includes
#include "include/sensorapi.h"

/*
 * Sensor Board Utility, Revision $rev:$
 *
 */

void thrusterCmd(int fd, int cmd)
{
    if(thrusterSafety(fd, cmd) != SB_OK)
        printf("Error safing thruster\n");
}




int main(int argc, char ** argv)
{
    if(argc < 2)
    {
        printf("LCD-related Commands:\n");
        printf("\tlcdshow -c  (clear screen)\n");
        printf("\tlcdshow -t line1 line2\n");
        printf("\tlcdshow -bloff (backlight on)\n");
        printf("\tlcdshow -blon  (backlight off)\n");
        printf("\tlcdshow -blfl  (backlight flash)\n");


        printf("\nSafety commands:\n");
        printf("\tlcdshow -safe (disable thrusters)\n");
        printf("\tlcdshow -unsafe (enable thrusters)\n");
        printf("\tlcdshow -diagon  (runtime diagnostics on)\n");
        printf("\tlcdshow -diagoff (runtime diagnostics off)\n");
	    printf("\tlcdshow -tstop (send zero speed command)\n");
        printf("\tlcdshow -safe [n] (safe thruster n)\n");
        printf("\tlcdshow -unsafe [n] (unsafe thruster n)\n");



        printf("\nOther commands:\n");
        printf("\tlcdshow -check (crude system check)\n");
        printf("\tlcdshow -status (show sensor readings)\n");
        printf("\tlcdshow -s  (begin start sequence)\n");

	    return -1;
    }

//    int fd = openSensorBoard("/dev/sensor");
    int fd = openSensorBoard("/dev/ttyUSB0");


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

    if(strcmp(argv[1], "-status") == 0)
    {
        int ret;
        unsigned char temp[NUM_TEMP_SENSORS];
        ret = readDepth(fd);

        if(ret == SB_ERROR)
            printf("Error reading depth!\n");
        else
            printf("Depth :   %d (%s)\n", ret, (ret > 50) ? "Seems OK" : "Check Sensor");

        ret = readTemp(fd, temp);
        if(ret == SB_ERROR)
            printf("Error reading temperature!\n");
        else
        {
            int i;
            printf("Temperature (C): ");

            for(i=0; i<NUM_TEMP_SENSORS; i++)
                if(temp[i] == 255)
                    printf("?? ");
                else
                    printf("%d ", temp[i]);
            printf("\n");
        }

        ret = readStatus(fd);
        if(ret == SB_ERROR)
            printf("Error reading board status!\n");
        else
        {
            printf("\nStatus: 0x%02X\n", ret);
            printf("\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n",
                (ret & STATUS_WATER) ? "Water present" : "No water detected.",
                (ret & STATUS_STARTSW) ? "Start switch on" : "Start switch off",
                (ret & STATUS_KILLSW) ? "Kill magnet present" : "No kill magnet",
                (ret & STATUS_BATT1) ? "Battery 1 active" : "Battery 1 inactive",
                (ret & STATUS_BATT2) ? "Battery 2 active" : "Battery 2 inactive",
                (ret & STATUS_BATT3) ? "Battery 3 active" : "Battery 3 inactive",
                (ret & STATUS_BATT4) ? "Battery 4 active" : "Battery 4 inactive",
                (ret & STATUS_BATT5) ? "Battery 5 active" : "Battery 5 inactive");
        }

        ret = readThrusterState(fd);
        if(ret == SB_ERROR)
            printf("Error reading thruster state!\n");
        else
        {
            printf("\nThruster state: 0x%02X\n", ret);
            printf("\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n",
            (ret & THRUSTER1_ENABLED) ? "Thruster 1 enabled" : "Thruster 1 disabled",
            (ret & THRUSTER2_ENABLED) ? "Thruster 2 enabled" : "Thruster 2 disabled",
            (ret & THRUSTER3_ENABLED) ? "Thruster 3 enabled" : "Thruster 3 disabled",
            (ret & THRUSTER4_ENABLED) ? "Thruster 4 enabled" : "Thruster 4 disabled",
            (ret & THRUSTER5_ENABLED) ? "Thruster 5 enabled" : "Thruster 5 disabled",
            (ret & THRUSTER6_ENABLED) ? "Thruster 6 enabled" : "Thruster 6 disabled");
        }

        ret = readBarState(fd);
        if(ret == SB_ERROR)
            printf("Error reading bar state!\n");
        else
        {
            printf("\nBar state: 0x%02X\n", ret);
            printf("\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n",
            (ret & BAR1_ENABLED) ? "Bar 1 enabled" : "Bar 1 disabled",
            (ret & BAR2_ENABLED) ? "Bar 2 enabled" : "Bar 2 disabled",
            (ret & BAR3_ENABLED) ? "Bar 3 enabled" : "Bar 3 disabled",
            (ret & BAR4_ENABLED) ? "Bar 4 enabled" : "Bar 4 disabled",
            (ret & BAR5_ENABLED) ? "Bar 5 enabled" : "Bar 5 disabled",
            (ret & BAR6_ENABLED) ? "Bar 6 enabled" : "Bar 6 disabled",
            (ret & BAR7_ENABLED) ? "Bar 7 enabled" : "Bar 7 disabled",
            (ret & BAR8_ENABLED) ? "Bar 8 enabled" : "Bar 8 disabled");
        }

    }


    if(strcmp(argv[1], "-check") == 0)
    {
        int ret = pingBoard(fd);

        if(ret == SB_OK)
            printf("Ping: OK\n");
        else
            printf("Ping: Error (Code %d)\n", ret);

        ret = checkBoard(fd);

        if(ret == SB_OK)
            printf("Self-test: OK\n");
        else
            printf("Self-test: Error (Code %d)\n", ret);

        close(fd);
        return 0;
    }



    if(strcmp(argv[1], "-safe") == 0)
    {
        if(argc == 3)
        {
            int t = atoi(argv[2]);
            if(t < 1 || t > 6)
            {
                printf("Bad thruster number: %d\n", t);
                close(fd);
                return -1;
            }

            unsigned int cmdList[]=
            {
                CMD_THRUSTER1_OFF, CMD_THRUSTER2_OFF, CMD_THRUSTER3_OFF,
                CMD_THRUSTER4_OFF, CMD_THRUSTER5_OFF, CMD_THRUSTER6_OFF
            };

            thrusterCmd(fd, cmdList[t-1]);

        } else
        {
            thrusterCmd(fd, CMD_THRUSTER1_OFF);
            thrusterCmd(fd, CMD_THRUSTER2_OFF);
            thrusterCmd(fd, CMD_THRUSTER3_OFF);
            thrusterCmd(fd, CMD_THRUSTER4_OFF);
            thrusterCmd(fd, CMD_THRUSTER5_OFF);
            thrusterCmd(fd, CMD_THRUSTER6_OFF);
        }


        close(fd);
        return 0;
    }

    if(strcmp(argv[1], "-unsafe") == 0)
    {
        if(argc == 3)
        {
            int t = atoi(argv[2]);
            if(t < 1 || t > 6)
            {
                printf("Bad thruster number: %d\n", t);
                close(fd);
                return -1;
            }

            unsigned int cmdList[]=
            {
                CMD_THRUSTER1_ON, CMD_THRUSTER2_ON, CMD_THRUSTER3_ON,
                CMD_THRUSTER4_ON, CMD_THRUSTER5_ON, CMD_THRUSTER6_ON
            };

            thrusterCmd(fd, cmdList[t-1]);

        } else
        {
            thrusterCmd(fd, CMD_THRUSTER1_ON);
            thrusterCmd(fd, CMD_THRUSTER2_ON);
            thrusterCmd(fd, CMD_THRUSTER3_ON);
            thrusterCmd(fd, CMD_THRUSTER4_ON);
            thrusterCmd(fd, CMD_THRUSTER5_ON);
            thrusterCmd(fd, CMD_THRUSTER6_ON);
        }


        close(fd);
        return 0;
    }

    if(strcmp(argv[1], "-tstop") == 0)
    {
        setSpeeds(fd, 0, 0, 0, 0, 0, 0);
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


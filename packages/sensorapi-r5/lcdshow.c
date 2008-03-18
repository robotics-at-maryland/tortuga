
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
    if(setThrusterSafety(fd, cmd) != SB_OK)
        printf("Error safing thruster\n");
}

void barCmd(int fd, int cmd)
{
    if(setBarState(fd, cmd) != SB_OK)
        printf("Error setting bar state\n");
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
        printf("\tlcdshow -safe [n] (safe thruster n)\n");
        printf("\tlcdshow -unsafe [n] (unsafe thruster n)\n");
        printf("\tlcdshow -diagon  (runtime diagnostics on)\n");
        printf("\tlcdshow -diagoff (runtime diagnostics off)\n");
	    printf("\tlcdshow -tstop (send zero speed command)\n");



        printf("\nOther commands:\n");
        printf("\tlcdshow -check (crude system check)\n");
        printf("\tlcdshow -status (show sensor readings)\n");
        printf("\tlcdshow -baron (enable bar outputs)\n");
        printf("\tlcdshow -baroff (disable bar outputs)\n");
        printf("\tlcdshow -baron [n] (enable bar output n)\n");
        printf("\tlcdshow -baroff [n] (disable bar output n)\n");
        printf("\tlcdshow -marker {1|2} (drop marker 1 or 2)\n");
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

        ret = readThrusterState(fd);
        if(ret == SB_ERROR)
            printf("Error reading thruster state!\n");
        else
        {
            printf("\nThruster state: 0x%02X\t", ret);
            printf("[%c%c%c%c%c%c]",
            (ret & THRUSTER1_ENABLED) ? '1' : '-',
            (ret & THRUSTER2_ENABLED) ? '2' : '-',
            (ret & THRUSTER3_ENABLED) ? '3' : '-',
            (ret & THRUSTER4_ENABLED) ? '4' : '-',
            (ret & THRUSTER5_ENABLED) ? '5' : '-',
            (ret & THRUSTER6_ENABLED) ? '6' : '-');

            if(ret == 0x00)
                printf("   (All safe)");

            if(ret == 0x3F)
                printf("   (All unsafe)");

        }

        ret = readOvrState(fd);
        if(ret == SB_ERROR)
            printf("\nError reading overcurrent register!\n");
        else
        {
            printf("\nOver-current: \t0x%02X", ret);
            printf("\t[%c%c%c%c%c%c]",
            (ret & THRUSTER1_OVR) ? '1' : '-',
            (ret & THRUSTER2_OVR) ? '2' : '-',
            (ret & THRUSTER3_OVR) ? '3' : '-',
            (ret & THRUSTER4_OVR) ? '4' : '-',
            (ret & THRUSTER5_OVR) ? '5' : '-',
            (ret & THRUSTER6_OVR) ? '6' : '-');

            if(ret == 0x00)
                printf("   (All ok)");

            if(ret == 0xFF)
                printf("   (All busted)");
        }


        ret = readBarState(fd);
        if(ret == SB_ERROR)
            printf("Error reading bar state!\n");
        else
        {
            printf("\nBar state: \t0x%02X", ret);
            printf("\t[%c%c%c%c%c%c%c%c]",
            (ret & BAR1_ENABLED) ? '1' : '-',
            (ret & BAR2_ENABLED) ? '2' : '-',
            (ret & BAR3_ENABLED) ? '3' : '-',
            (ret & BAR4_ENABLED) ? '4' : '-',
            (ret & BAR5_ENABLED) ? '5' : '-',
            (ret & BAR6_ENABLED) ? '6' : '-',
            (ret & BAR7_ENABLED) ? '7' : '-',
            (ret & BAR8_ENABLED) ? '8' : '-');

            if(ret == 0x00)
                printf(" (All off)");

            if(ret == 0xFF)
                printf(" (All on)");
        }

        ret = readStatus(fd);
        if(ret == SB_ERROR)
            printf("Error reading board status!\n");
        else
        {
            printf("\n\nStatus: 0x%02X\n", ret);
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

        struct powerInfo info;

        ret = readMotorCurrents(fd, &info);
        if(ret == SB_OK)
        {
            int i;
            printf("\nOutput currents:\n\t[1-4]: ");
            for(i=0; i<4; i++)
                printf("\t%2.3fA", info.motorCurrents[i]);
            printf("\n\t[5-8]: ");
            for(i=0; i<4; i++)
                printf("\t%2.3fA", info.motorCurrents[i+4]);
        } else
            printf("\nError reading motor currents\n");

        printf("\n");
        ret = readBoardVoltages(fd, &info);
        if(ret == SB_OK)
        {
            printf("\nPower information:\n");
            printf("\t5V Bus :\t% 2.3fV\t  %2.3fA\n", info.v5VBus, info.i5VBus);
            printf("\t12V Bus:\t%2.3fV\t  %2.3fA\n", info.v12VBus, info.i12VBus);
            printf("\tAux Current:\t %2.3fA\n", info.iAux);
        } else
            printf("\nError reading power information\n");
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

    if(strcmp(argv[1], "-marker") == 0)
    {
        if(argc == 3)
        {
            int t = atoi(argv[2]);
            if(t != 1 && t != 2)
            {
                printf("Bad marker number: %d\n", t);
                close(fd);
                return -1;
            }

            dropMarker(fd, t-1);

        } else
        {
            printf("Which marker? Specify 1 or 2.\n");
        }

        close(fd);
        return 0;
    }

    if(strcmp(argv[1], "-safe") == 0)
    {
        int i;
        unsigned int cmdList[]=
        {
            CMD_THRUSTER1_OFF, CMD_THRUSTER2_OFF, CMD_THRUSTER3_OFF,
            CMD_THRUSTER4_OFF, CMD_THRUSTER5_OFF, CMD_THRUSTER6_OFF
        };

        if(argc == 3)
        {
            int t = atoi(argv[2]);
            if(t < 1 || t > 6)
            {
                printf("Bad thruster number: %d\n", t);
                close(fd);
                return -1;
            }

            thrusterCmd(fd, cmdList[t-1]);

        } else
        {
            for(i=0; i<6; i++)
                thrusterCmd(fd, cmdList[i]);
        }

        close(fd);
        return 0;
    }

    if(strcmp(argv[1], "-unsafe") == 0)
    {
        int i;
        unsigned int cmdList[]=
        {
            CMD_THRUSTER1_ON, CMD_THRUSTER2_ON, CMD_THRUSTER3_ON,
            CMD_THRUSTER4_ON, CMD_THRUSTER5_ON, CMD_THRUSTER6_ON
        };

        if(argc == 3)
        {
            int t = atoi(argv[2]);
            if(t < 1 || t > 6)
            {
                printf("Bad thruster number: %d\n", t);
                close(fd);
                return -1;
            }

            thrusterCmd(fd, cmdList[t-1]);

        } else
        {
            for(i=0; i<6; i++)
                thrusterCmd(fd, cmdList[i]);
        }


        close(fd);
        return 0;
    }



    if(strcmp(argv[1], "-baroff") == 0)
    {
        int i;
        unsigned int cmdList[]=
        {
            CMD_BAR1_OFF, CMD_BAR2_OFF, CMD_BAR3_OFF, CMD_BAR4_OFF,
            CMD_BAR5_OFF, CMD_BAR6_OFF, CMD_BAR7_OFF, CMD_BAR8_OFF
        };

        if(argc == 3)
        {
            int t = atoi(argv[2]);
            if(t < 1 || t > 8)
            {
                printf("Bad bar number: %d\n", t);
                close(fd);
                return -1;
            }

            barCmd(fd, cmdList[t-1]);

        } else
        {
            for(i=0; i<8; i++)
                barCmd(fd, cmdList[i]);
        }

        close(fd);
        return 0;
    }



    if(strcmp(argv[1], "-baron") == 0)
    {
        int i;
        unsigned int cmdList[]=
        {
            CMD_BAR1_ON, CMD_BAR2_ON, CMD_BAR3_ON, CMD_BAR4_ON,
            CMD_BAR5_ON, CMD_BAR6_ON, CMD_BAR7_ON, CMD_BAR8_ON
        };

        if(argc == 3)
        {
            int t = atoi(argv[2]);
            if(t < 1 || t > 8)
            {
                printf("Bad bar number: %d\n", t);
                close(fd);
                return -1;
            }

            barCmd(fd, cmdList[t-1]);

        } else
        {
            for(i=0; i<8; i++)
                barCmd(fd, cmdList[i]);
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


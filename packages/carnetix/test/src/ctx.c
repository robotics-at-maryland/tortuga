/*****************************************************************************
 * Steve Moskovchenko  (aka evil wombat)                                     *
 * University of Maryland                                                    *
 * stevenm86 at gmail dot com                                                *
 * http://wam.umd.edu/~stevenm/carnetix.html                                 *
 *                                                                           *
 * Linux and Mac OS API for the Carnetix P2140 power supply.                 *
 * The program uses libusb, which works on Mac OS X, Linux, and Windows.     *
 *                                                                           *
 * Released under the BSD License. See License.txt for info.                 *
 *                                                                           *
 * Version 0.2. Enjoy!                                                       *
 *                                                                           *
 ****************************************************************************/

/*
 * This is just a small demo of the API. It just gets the values and parameters,
 * and prints them to the console. All the really important stuff is in ctxapi.c
 * and ctxapi.h. ctxapi.h also contains the documentation for all the functions.
 */


#include <stdio.h>
#include <stdlib.h>
#include <usb.h>
#include "ctxapi.h"



int ctxPrintValues(struct ctxValues * val)
{
    if(!val)
        return -1;

    printf("Batt Voltage: %f V\nBatt Current: %f A\n\n", val->battVoltage, val->battCurrent);
    printf("Pri Voltage : %f V\nPri Current : %f A\n\n", val->priVoltage, val->priCurrent);
    printf("Sec Voltage : %f V\nSec Current : %f A\n\n", val->secVoltage, val->secCurrent);
    printf("Temperature : %f C\n", val->temperature);
    printf("State: %d (%s)\n", val->state, ctxStateNameToText(val->state));
    printf("\nLED State (0x%04X):\n", val->ledValue);
    if(val->ledValue & LED_IGNITION)
        printf("\tIgnition\n");

    if(val->ledValue & LED_PULSESTART)
        printf("\tPulse Start\n");

    if(val->ledValue & LED_FANFAULT)
        printf("\tFan Fault\n");

    if(val->ledValue & LED_DELAYON)
        printf("\tDelay On\n");

    if(val->ledValue & LED_ACPI)
        printf("\tACPI\n");

    if(val->ledValue & LED_POWERGOOD)
        printf("\tPower Good\n");

    if(val->ledValue & LED_PS_ON)
        printf("\tPower Supply On\n");

    if(val->ledValue & LED_FANON)
        printf("\tFan On\n");

    if(val->ledValue & LED_VOLTAGE_OK)
        printf("\tBattery Voltage OK\n");

    if(val->ledValue & LED_PRICUR_OK)
        printf("\tPrimary Current OK\n");

    if(val->ledValue & LED_SECCUR_OK)
        printf("\tSecondary Current OK\n");

    return 0;
}



int ctxPrintParams(struct ctxParams * prm)
{
    if(!prm)
        return -1;

    printf("SD_DELAY : %.1f sec\n", prm->sd_dly);
    printf("DMT      : %.1f hr\n", prm->dmt);
    printf("DLYON    : %.1f sec\n", prm->dlyon);
    printf("BU_LO    : %.1f sec\n", prm->bu_lo);
    printf("SD_LO    : %.1f sec\n", prm->sd_lo);
    printf("LOBATT   : %f V\n", prm->lobatt);
    printf("Jumpers  : 0x%02X\n", prm->softJumpers);

    printf("ACPI Dly : %.1f sec\n", prm->acpiDelay);
    printf("ACPI Dur : %.1f sec\n", prm->acpiDuration);
    printf("Low Temp : %f C\n", prm->lowTemp);

    return 0;
}



int main(int argc, char ** argv)
{
    struct ctxValues val;
    struct ctxParams prm;
    char buf[25];

    printf("Carnetix P2140 Linux Tool\nbrought to you by an evil wombat\nHere goes nothing...\n");

    if(argc == 1)
    {
        printf("Usage: ctxutil COMMAND\n");
        printf("Commands are:\n\t-pri-on \n\t-pri-off\n\t-sec-on\n\t-sec-off\n\t-p5v-on\n\t-p5v-off\n\t-stat\n\t-config\n");
        return -1;
    }


    usb_dev_handle * hDev  = ctxInit();

    if(!hDev)
    {
        printf("\nNo device!\n");
        return -1;
    }

    if(strcmp(argv[1], "-stat") == 0)
    {
        ctxGetFWVersion(hDev, buf, 25);
        printf("Firmware Version is: %s\n", buf);

        if(ctxReadValues(hDev, &val) == 0)
        {
            printf("\nPower Supply Readings:\n");
            ctxPrintValues(&val);
        } else
        {
            printf("Error reading values.\n");
            ctxClose(hDev);
            return -1;
        }
    }

    if(strcmp(argv[1], "-config") == 0)
    {
        ctxGetFWVersion(hDev, buf, 25);
        printf("Firmware Version is: %s\n", buf);

        if(ctxReadParams(hDev, &prm) == 0)
        {
            printf("\nPower Supply Configuration:\n");
            ctxPrintParams(&prm);
        } else
        {
            printf("Error reading configuration.\n");
            ctxClose(hDev);
            return -1;
        }
    }

    if(strcmp(argv[1], "-pri-on") == 0)
    {
        if(ctxPriOn(hDev) == 0)
            printf("Primary supply on.\n");
        else
            printf("Error enabling primary supply.\n");
    }


    if(strcmp(argv[1], "-pri-off") == 0)
    {
        if(ctxPriOff(hDev) == 0)
            printf("Primary supply off.\n... but for some reason we are still here.\n");
        else
            printf("Error disabling primary supply.\n");
    }


    if(strcmp(argv[1], "-sec-on") == 0)
    {
        if(ctxSecOn(hDev) == 0)
            printf("Secondary supply on.\n");
        else
            printf("Error enabling secondary supply.\n");
    }

    if(strcmp(argv[1], "-sec-off") == 0)
    {
        if(ctxSecOff(hDev) == 0)
            printf("Secondary supply off.\n");
        else
            printf("Error disabling secondary supply.\n");
    }

    if(strcmp(argv[1], "-p5v-on") == 0)
    {
        if(ctxP5VOn(hDev) == 0)
            printf("P5V supply on.\n");
        else
            printf("Error enabling P5V supply.\n");
    }

    if(strcmp(argv[1], "-p5v-off") == 0)
    {
        if(ctxP5VOff(hDev) == 0)
            printf("P5V supply off.\n");
        else
            printf("Error disabling P5V supply.\n");
    }

    ctxClose(hDev);
    return 0;
}

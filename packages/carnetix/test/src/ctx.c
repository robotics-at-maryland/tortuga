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

    usb_dev_handle * hDev  = ctxInit();

    if(!hDev)
    {
        printf("\nNo device!\n");
        return -1;
    }


    //ctxPriOff(hDev);

    printf("\nYou should probably upgrage to Firmware 1.8+ if you haven't already done so.\n\n");

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

    ctxReadParams(hDev, &prm);

    printf("\nPower Supply Configuration:\n");
    ctxPrintParams(&prm);

    ctxClose(hDev);
    return 0;
}

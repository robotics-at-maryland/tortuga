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
    printf("Temperature : %f C\n\n", val->temperature);
    printf("LED Status  : 0x%04X\nState: %d [%s]\n\n", val->ledValue, val->state, ctxStateNames[val->state]);
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
    printf("LOBATT   : %.1f V\n", prm->lobatt);
    printf("Jumpers  : 0x%02X\n", prm->softJumpers);

    printf("ACPI Dly : %.1f sec\n", prm->acpiDelay);
    printf("ACPI Dur : %.1f sec\n", prm->acpiDuration);
    printf("Low Temp : %.1f C\n", prm->lowTemp);
}

int main(int argc, char ** argv)
{
    unsigned char buf[25];
    unsigned char get[] = {0x40,0x17};

    printf("Carnetix P2140 Linux Tool\nbrought to you by an evil wombat\nHere goes nothing...\n");
    usb_dev_handle * hDev  = ctxInit();

    if(!hDev)
    {
        printf("\nNo device!\n");
        return -1;
    }

    struct ctxValues val;
    struct ctxParams prm;


    printf("\nYou should probably upgrage to Firmware 1.8 if you haven't already done so.\n\n");

    ctxGetFWVersion(hDev, buf, 25);
    printf("Firmware Version is: %s\n", buf);

    ctxReadValues(hDev, &val);
    ctxPrintValues(&val);

    ctxReadParams(hDev, &prm);
    ctxPrintParams(&prm);

    ctxClose(hDev);
    return 0;
}

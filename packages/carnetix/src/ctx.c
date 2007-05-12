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
    printf("LED Status  : 0x%04X\nState: %d\n\n", val->ledValue, val->state);
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

    printf("\n");
    ctxReadValues(hDev, &val);
    ctxPrintValues(&val);

    return 0;
}

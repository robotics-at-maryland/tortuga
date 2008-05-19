#include <p30fxxxx.h>
#include "buscodes.h"
#include <stdio.h>

#define SENSORBOARD_BFIN
#include "uart.c"

_FOSC( CSW_FSCM_OFF & HS); //EC_PLL4); //ECIO );
_FWDT ( WDT_OFF );


#define TRIS_OUT 0
#define TRIS_IN  1
#define byte unsigned char

#define LED_ON      1

/* LED pin definitions */
#define LAT_LED1    _LATE3
#define TRIS_LED1   _TRISE3

#define LAT_LED2    _LATE4
#define TRIS_LED2   _TRISE4

#define LAT_LED3    _LATE5
#define TRIS_LED3   _TRISE5

/* THIS SHOULD NEVER BE DRIVEN HIGH */
#define LAT_PIC_FPGA_RESET  _LATD1
#define TRIS_PIC_FPGA_RESET _TRISD1

/* THIS SHOULD NEVER BE DRIVEN HIGH OR ELSE */
#define LAT_BF_RESET    _LATD3
#define TRIS_BF_RESET   _TRISD3

void main()
{
    LAT_BF_RESET = 0;
    TRIS_BF_RESET = TRIS_OUT;

    LAT_PIC_FPGA_RESET = 0;
    TRIS_PIC_FPGA_RESET = TRIS_IN;

    TRIS_LED1 = TRIS_OUT;
    TRIS_LED2 = TRIS_OUT;
    TRIS_LED3 = TRIS_OUT;

    LAT_LED1 = LED_ON;
    LAT_LED2 = LED_ON;
    LAT_LED3 = LED_ON;

    while(1);
}

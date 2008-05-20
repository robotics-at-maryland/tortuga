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
#define LAT_LED_GREEN    _LATE3
#define TRIS_LED_GREEN   _TRISE3

#define LAT_LED_YELLOW    _LATE4
#define TRIS_LED_YELLOW   _TRISE4

#define LAT_LED_RED    _LATE5
#define TRIS_LED_RED   _TRISE5


/* BF and FPGA reset pins */

/* THIS SHOULD NEVER BE DRIVEN HIGH */
#define LAT_PIC_FPGA_RESET  _LATD1
#define TRIS_PIC_FPGA_RESET _TRISD1

/* THIS SHOULD NEVER BE DRIVEN HIGH OR ELSE */
#define LAT_BF_RESET    _LATD3
#define TRIS_BF_RESET   _TRISD3


/* UART Pin specifications */
#define LAT_U1_TX   _LATF3
#define TRIS_U1_TX  _TRISF3
#define LAT_U1_RX   _LATF2
#define TRIS_U1_RX  _TRISF2

#define LAT_U2_TX   _LATF5
#define TRIS_U2_TX  _TRISF5
#define LAT_U2_RX   _LATF4
#define TRIS_U2_RX  _TRISF4

#define IN_U1_RX    _RF2

void main()
{
    long l;
    LAT_BF_RESET = 0;
    TRIS_BF_RESET = TRIS_OUT;

    TRIS_U1_TX = TRIS_IN;
    TRIS_U1_RX = TRIS_IN;
    TRIS_U2_TX = TRIS_IN;
    TRIS_U2_RX = TRIS_IN;


    LAT_PIC_FPGA_RESET = 0;
    TRIS_PIC_FPGA_RESET = TRIS_IN;

    TRIS_LED_GREEN = TRIS_OUT;
    TRIS_LED_YELLOW = TRIS_OUT;
    TRIS_LED_RED = TRIS_OUT;

    LAT_LED_GREEN = ~LED_ON;
    LAT_LED_YELLOW = LED_ON;
    LAT_LED_RED = LED_ON;


    while(IN_U1_RX != 0);            /* Wait for FPGA to initialize */
    LAT_LED_RED = ~LED_ON;

    for(l=0; l<200000; l++);     /* Wait a little bit more */
    LAT_LED_YELLOW = ~LED_ON;
    LAT_LED_GREEN = LED_ON;


    TRIS_BF_RESET = TRIS_IN;    /* Start the Blackfin */

    while(1);
}

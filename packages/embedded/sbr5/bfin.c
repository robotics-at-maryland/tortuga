#include <p30fxxxx.h>
#include "buscodes.h"
#include <stdio.h>

#define SENSORBOARD_BFIN
#include "uart.c"

_FOSC( CSW_FSCM_OFF & XT_PLL8); //EC_PLL4); //ECIO );
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


#define TRIS_PWM    _TRISD2

void _ISR _U2RXInterrupt(void)
{
    byte t;
    IFS1bits.U2RXIF = 0;    /* Clear RX interrupt */


//    t = U2RXREG;

//    LAT_LED_GREEN = t & 0x01;
//    LAT_LED_YELLOW = t & 0x02;
//    LAT_LED_RED = t & 0x04;



    TMR2 = 0;
    OC3RS = ((unsigned char)(U2RXREG & 0xFF));


}

void initUart()
{
    _TRISF5 = 0;        // Un-tristate the U2 transmit pin
    U2MODE = 0x0000;
    U2BRG = 10;  /* 10 for 115200 at 20 MIPS */
    U2MODEbits.UARTEN = 1;
    U2STAbits.UTXEN = 1;   // Enable transmit
    U2STAbits.UTXISEL=1;    /* Generate interrupt only when buffer is empty */
    U2STAbits.URXISEL=0;    /* Generate interrupt when a byte comes in */
    IEC1bits.U2RXIE = 1;    /* Enable RX interrupt */
}

void initJPortal()
{
    TRIS_PWM = 0;        /* Un-tristate the PWM output */
    OC3CON = 0x0000;
    OC3CONbits.OCTSEL = 0;  /* Use Timer2 */
    OC3CONbits.OCM = 0x6;   /* PWM, fault pin disabled */
    PR2 = 255;
    OC3RS = 0;
    T2CONbits.TCKPS = 0;    /* No prescaler */
    T2CONbits.TON = 1;      /* Start Timer2 */
}

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

    for(l=0; l<1600000; l++);     /* Wait a little bit more */
    LAT_LED_YELLOW = ~LED_ON;
    LAT_LED_GREEN = LED_ON;


    TRIS_BF_RESET = TRIS_IN;    /* Start the Blackfin */

    for(l=0; l<1600000; l++);     /* Wait a little bit more before accepting commands */

    initUart();
    initJPortal();

    OC3RS = 128;
    while(1);
}

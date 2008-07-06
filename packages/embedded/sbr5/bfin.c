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


/* Bus pin assignments */
#define REQ_CN_BIT  (CNEN1bits.CN1IE)
#define IN_REQ      _RC13
#define TRIS_REQ    _TRISC13

#define LAT_AKN     _LATB8
#define TRIS_AKN    _TRISB8

#define IN_RW       _RC14
#define TRIS_RW     _TRISC14

#define RW_READ     0
#define RW_WRITE    1


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


/* Transmit buffer */
#define TXBUF_LEN 30
byte txBuf[TXBUF_LEN];
byte txPtr = 0;



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


#define STATE_TOP_LEVEL     0
#define STATE_READ_CMD      1
#define STATE_WRITE_CMD     2
#define STATE_WRITE_LCD     3
#define STATE_SETSPEED_U1   4
#define STATE_SETSPEED_U2   5

byte busState = 0;
byte nParam = 0;
byte p1=0;

byte cfgRegs[16];


/* If Master writes us data, this gets called */
void processData(byte data)
{
    txPtr = 0;

    switch(busState)
    {
        case STATE_TOP_LEVEL:     /* New commands */
        {
            switch(data)
            {
                case BUS_CMD_PING:
                {
                    txBuf[0] = 0;
                    break;
                }

                case BUS_CMD_ID:
                {
                    txBuf[0] = sprintf(txBuf+1, "Blackfin Board");
                    break;
                }

                case BUS_CMD_READ_REG:
                {
                    busState = STATE_READ_CMD;
                    nParam = 0;
                    break;
                }

                case BUS_CMD_WRITE_REG:
                {
                    busState = STATE_WRITE_CMD;
                    nParam = 0;
                    break;
                }

                case BUS_CMD_SETSPEED_U1:
                {
                    busState = STATE_SETSPEED_U1;
                    nParam = 0;
                    break;
                }

                case BUS_CMD_SETSPEED_U2:
                {
                    busState = STATE_SETSPEED_U2;
                    nParam = 0;
                    break;
                }
            }
            break;
        }

        case STATE_READ_CMD:
        {
            nParam = 0;
            busState = STATE_TOP_LEVEL;
            txBuf[0] = 1;
            txBuf[1] = cfgRegs[data];
            break;
        }

        case STATE_WRITE_CMD:
        {
            if(nParam == 0)
                p1 = data;

            nParam++;

            if(nParam == 2)
            {
                nParam=0;
                busState = STATE_TOP_LEVEL;
                cfgRegs[p1] = data;
            }
            break;
        }
    }
}

/*
 * Checks if we have an incoming request. If so, handles it.
 * Returns 0 if no request was waiting. Returns 1 if request was handled.
 */
byte checkBus()
{
    byte data=0;

    /* No request - no action. */
    if(IN_REQ == 0)
        return 0;

    /* Data is coming in */
    if(IN_RW == RW_WRITE)
    {
        /* Read data */
        data = readBus();

        /* Acknowledge read */
        LAT_AKN = 1;
        TRIS_AKN = TRIS_OUT;

        /* Wait for Request to drop */
        /* Need a timeout here to detect Master's fault */
        while(IN_REQ);

        /* Drop Akn */
        TRIS_AKN = TRIS_IN;

        processData(data);
    } else
    {
        /* Master requests a byte */

        /* Is there data left in the buffer? */
        if(txPtr >= TXBUF_LEN)
        {
            /* Error Condition! We have nothing to write */
        }

        data = txBuf[txPtr++];

        /* Put data on the bus */
        writeBus(data);

        /* Acknowledge write */
        LAT_AKN = 1;
        TRIS_AKN = TRIS_OUT;

        /* Wait for Request to drop */
        /* Need a timeout here to detect Master's fault */
        while(IN_REQ);

        /* Release bus first */
        freeBus();

        /* Finally, release Akn */
        TRIS_AKN = TRIS_IN;
    }
    return 1;
}


/* Read a byte from the bus */
byte readBus()
{
    return (PORTB & 0xFF);
}


/* Take bus out of high-impedance state and write a byte there */
void writeBus(byte b)
{
    TRISB = TRISB & 0xFF00;
    LATB = (LATB & 0xFF00) | b;
}


/* Put bus in high-impedance state. */
void freeBus()
{
    TRISB = TRISB | 0xFF;
}

void enableBusInterrupt()
{
    REQ_CN_BIT = 1; /* Turn on CN for the pin */
    checkBus();
}

void disableBusInterrupt()
{
    REQ_CN_BIT = 0;    /* Turn off CN for the pin */
}


/* Initialize the CN interrupt to watch the Req line */
void initCN()
{
    enableBusInterrupt();
    IPC3bits.CNIP = 4;      /* Raise CN interrupt priority above ADC */

    IPC2bits.U1TXIP = 6;    /* TX at priority 6 */
    IPC2bits.U1RXIP = 5;    /* RX at priority 5 */

    IPC6bits.U2TXIP = 6;    /* TX at priority 6 */
    IPC6bits.U2RXIP = 5;    /* RX at priority 5 */


    IFS0bits.CNIF = 0;      /* Clear CN interrupt flag */
    IEC0bits.CNIE = 1;      /* Turn on CN interrupts */
}


void initBus()
{
    /* Put everything in high-impedance state */
    freeBus();
    TRIS_RW = TRIS_IN;
    TRIS_REQ = TRIS_IN;
    TRIS_AKN = TRIS_IN;
    initCN();
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

    initBus();


    while(IN_U1_RX != 0);            /* Wait for FPGA to initialize */
    LAT_LED_RED = ~LED_ON;

    for(l=0; l<1600000; l++);     /* Wait a little bit more */
    LAT_LED_YELLOW = ~LED_ON;
    LAT_LED_GREEN = LED_ON;


    TRIS_BF_RESET = TRIS_IN;    /* Start the Blackfin */

    for(l=0; l<1600000; l++);     /* Wait a little bit more before accepting commands */

    initUart();

/*
    // Uncomment this for portal mode
    initJPortal();
    OC3RS = 128;
    while(1);
*/

    while(1);

}

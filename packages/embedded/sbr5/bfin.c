#include <p30fxxxx.h>
#include "buscodes.h"
#include <stdio.h>

#define SENSORBOARD_BFIN
#include "uart.c"

_FOSC( CSW_FSCM_OFF & XT_PLL8 & PWRT_64); //EC_PLL4); //ECIO );
_FWDT ( WDT_OFF );



/* Instead of sonaring, use jportal */
// #define PORTAL

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

byte sonarBuf[SONAR_PACKET_LEN]; /* Put vector here */
byte sonarRxBuf[SONAR_PACKET_LEN];


byte portalMode = 0;


byte fCount = 0;
byte sonarPtr = 0;

void disableUartInterrupt();
void enableUartInterrupt();


void _ISR _U2RXInterrupt(void)
{
    IFS1bits.U2RXIF = 0;    /* Clear RX interrupt */

    if(portalMode)
    {
        TMR2 = 0;
        OC3RS = ((unsigned char)(U2RXREG & 0xFF));
    } else
    {

        byte t = U2RXREG;
	byte i;
        actLight(0);

        if(sonarPtr < SONAR_PACKET_LEN)
            sonarRxBuf[sonarPtr++] = t;   /* Blargh */

	if(sonarPtr >= SONAR_PACKET_LEN)
	{
	    for(i=0; i<SONAR_PACKET_LEN; i++)
		sonarBuf[i] = sonarRxBuf[i];
	}


        /* We are looking for 6 FFs in a row. */
        if(t == 0xFF)
        {
            fCount++;
            /* Start of sequence? */
            if(fCount >= 6)
                sonarPtr = 0;
        } else
            fCount = 0;
    }
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
    _U2RXIE = 1;    /* Enable RX interrupt */
}

void initJPortal()
{
    TRIS_PWM = 0;        /* Un-tristate the PWM output */
    OC3CON = 0x0000;
    OC3CONbits.OCTSEL = 0;  /* Use Timer2 */
    IEC0bits.T2IE = 0;      /* Disable T2 interrupt */
    IEC0bits.CNIE = 0;      /* Disable CN interrupts */
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

                case BUS_CMD_BFRESET:
                {
                    int l;
                    TRIS_BF_RESET = TRIS_OUT;   /* Stop the Blackfin */
                    for(l=0; l<1000; l++);     /* Wait a little bit. Yes, but I don't care */
                    TRIS_BF_RESET = TRIS_IN;    /* Start the Blackfin */

                    break;
                }

                case BUS_CMD_SONAR:
                {
                    int i;

                    _U2RXIE = 0;
		    for(i=0; i<SONAR_PACKET_LEN; i++)
                        txBuf[i+1] = sonarBuf[i];
                    txBuf[0] = SONAR_PACKET_LEN;
                    _U2RXIE = 1;
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

/* Read a byte from the bus */
byte readBus()
{
    return PORTB;
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



void _ISR _CNInterrupt(void)
{
    IFS0bits.CNIF = 0;      /* Clear CN interrupt flag */

    /* Don't check bus if its interrupt is disabled. Avoids a race condition */
    if(REQ_CN_BIT == 1)
    {
        checkBus();
        actLight(1);
    }
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
}


void actLight(byte l)
{
    if(!portalMode)
    {
        PR2 = 800;            /* Period */
        TMR2 = 0;               /* Reset timer */
        IFS0bits.T2IF = 0;      /* Clear interrupt flag */
        IEC0bits.T2IE = 1;      /* Enable interrupts */
        T2CONbits.TCS = 0;      /* Use internal clock */
        T2CONbits.TCKPS = 3;    /* 1:256 prescaler */
        T2CONbits.TON = 1;      /* Start Timer2 */

        if(l == 0)
            LAT_LED_GREEN = LED_ON;
        else
            LAT_LED_YELLOW = LED_ON;
    }
}


/* ISR for Timer2. Used for making the ACT light pretty */
void _ISR _T2Interrupt(void)
{
    IFS0bits.T2IF = 0;      /* Clear interrupt flag */
    if(!portalMode)
    {
        IEC0bits.T2IE = 0;      /* Disable interrupts */
        LAT_LED_GREEN = ~LED_ON;
        LAT_LED_YELLOW = ~LED_ON;
        T2CONbits.TON = 0;  /* Stop Timer1 */
    }
}


int main()
{
    long l;
    ADPCFG = 0xFFFF;
    TRISB = 0xFFFF;

    LAT_BF_RESET = 0;
    TRIS_BF_RESET = TRIS_OUT;

    TRIS_RW = TRIS_IN;
    TRIS_REQ = TRIS_IN;
    TRIS_AKN = TRIS_IN;


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

    _TRISF3 = TRIS_IN;
    _TRISF2 = TRIS_OUT;
    _LATF2 = 1;

    for(l=0; l<1600000; l++);     /* Wait a little bit more before accepting commands */

    initUart();

    /* Check portal jumper */




    if(_RF3)  /* Jumper between PGD and PGC */
    {
        portalMode = 1;
        LAT_LED_GREEN = LED_ON;
        LAT_LED_YELLOW = LED_ON;
        LAT_LED_RED = LED_ON;
        disableBusInterrupt();
        initJPortal();
        OC3RS = 128;
        _TRISF2 = TRIS_IN;
        while(1);
    } else
        portalMode = 0;

    _TRISF2 = TRIS_IN;

    // Uncomment this for portal mode
    initCN();


    byte i;
    for(i=0; i<SONAR_PACKET_LEN; i++)
        sonarBuf[i] = 0;

    while(1);
}

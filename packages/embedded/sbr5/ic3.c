/***************
* Parts of the I2C code in this program came from a Microchip Application Note.
* License says we can use it with Microchip products.
***************/


#include <p30fxxxx.h>
#include <string.h>
#include "buscodes.h"

#define SENSORBOARD_IC3
#include "uart.c"
#include "i2c.c"

//_FOSC( CSW_FSCM_OFF & FRC );
_FOSC( CSW_FSCM_OFF & ECIO & PWRT_64);
//_FOSC( FRC_LO_RANGE);
//_FOSCSEL(FRC);
//_FPOR( PWRT_OFF);
//Turn off the watchdog
_FWDT ( WDT_OFF );

#define TRIS_OUT 0
#define TRIS_IN  1
#define byte unsigned char

/*
 * Bus = D1 D0 E5-E0
 * Req = B0
 * Akn = D2
 * RW  = E8 again
 */

/* Bus pin assignments */
#define REQ_CN_BIT  (CNEN1bits.CN2IE)

#define IN_REQ      _RB0
#define TRIS_REQ    _TRISB0

#define LAT_AKN     _LATD2
#define TRIS_AKN    _TRISD2

#define IN_RW       _RE8
#define TRIS_RW     _TRISE8

#define RW_READ     0
#define RW_WRITE    1



/* Transmit buffer */
#define TXBUF_LEN 60
byte txBuf[TXBUF_LEN];
byte txPtr = 0;


/* Temp = Temperature */
#define TEMP_DATA_SIZE 5
byte tempData[TEMP_DATA_SIZE];

/*
 * Configuration Registers
 * These are general-purpose settings registers that the Master can read and write.
 * This could be sonar sampling rate and target frequencies, any sort of calibration
 * values, etc.
 */
byte cfgRegs[16];


/*
 * Bus states
 * Some commands from Master may have one or more arguments.
 * 'Top Level' indicates that next incoming byte is treated as a command.
 * Other values indicate that the next byte should be treated as an argument
 * to a command issued earlier. Variables below are used to track how many
 * arguments have been received, and what their values were.
 */
#define STATE_TOP_LEVEL     0
#define STATE_READ_CMD      1
#define STATE_WRITE_CMD     2




byte busState = 0;
byte nParam = 0;
byte p1=0;




/* If Master writes us data, this gets called */
void processData(byte data)
{
    txPtr = 0;
    txBuf[1] = 9;

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
                    txBuf[0] = sprintf(txBuf+1, "TMP STA");
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

                case BUS_CMD_STARTSW:
                {
                    txBuf[0] = 1;
                    txBuf[1] = ~_RB1;    /* Just the start switch ... */

                    break;
                }

                case BUS_CMD_TEMP:
                {
                    byte i;
                    for(i=0; i<TEMP_DATA_SIZE; i++)
                        txBuf[i+1] = tempData[i];

                    txBuf[0] = TEMP_DATA_SIZE;
                    break;
                }
            }
        }
        break;

        case STATE_READ_CMD:
        {
            nParam = 0;
            busState = STATE_TOP_LEVEL;
            txBuf[0] = 1;
            txBuf[1] = cfgRegs[data];
        }
        break;

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

        }
        break;

    }
}


/* Read a byte from the bus */
byte readBus()
{
    return (PORTE & 0x3F) | (_RD0 << 6) | (_RD1 << 7);
}


/* Take bus out of high-impedance state and write a byte there */
void writeBus(byte b)
{
    TRISE = TRISE & 0xFFC0;
    _TRISD1 = TRIS_OUT;
    _TRISD0 = TRIS_OUT;

     LATE = (LATE & 0xFFC0) | (b & 0x3F);
    _LATD0 = (b & 0x40) >> 6;
    _LATD1 = (b & 0x80) >> 7;

}


/* Put bus in high-impedance state. */
void freeBus()
{
    _TRISD1 = TRIS_IN;
    _TRISD0 = TRIS_IN;
    TRISE = TRISE | 0x3F;
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

/*
 * These functions are insanely simple. But they are made anyway to prevent
 * a race condition when the bus code tries to send back partially-written data.
 *
 * The names are misleading since the CN interrupt is not actually disabled, but
 * rather the CN feature of just the Req line is turned off. Some other system on
 * this Slave may require the use of CN so disabling it entirely is a bit extreme.
 */
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

/* This can lead to same interrupt priority issues as with depth in rev 3 */
//     IPC3bits.CNIP = 6;      /* Raise CN interrupt priority above ADC */
//     IFS0bits.CNIF = 0;      /* Clear CN interrupt flag */
    IEC0bits.CNIE = 1;      /* Turn on CN interrupts */

/* Same priorities as depth chip. */
/* UART priorities above CN       */
    IPC2bits.U1TXIP = 6;    /* TX at priority 6 */
    IPC2bits.U1RXIP = 5;    /* RX at priority 5 */

    IPC6bits.U2TXIP = 6;    /* TX at priority 6 */
    IPC6bits.U2RXIP = 5;    /* RX at priority 5 */


    IPC3bits.CNIP = 4;      /* Bus at priority 4 */
    IPC2bits.ADIP = 2;      /* ADC at priority 2 */
}


/*
 * Put bus in the idle state. This should be done as soon as possible to prevent
 * pins in unknown states from interfering with bus operations. The Master should probably
 * wait a few cycles upon power-up to allow all Slaves to release the bus before trying to
 * use it.
 */
void initBus()
{
    /* Put everything in high-impedance state */
    freeBus();
    TRIS_RW = TRIS_IN;
    TRIS_REQ = TRIS_IN;
    TRIS_AKN = TRIS_IN;
    initCN();
}



/*
 * ISR for the CN interrupt. Req line is CN-based to minimize bus latency.
 * Code for dealing with other CN pins should be placed here as well.
 */
void _ISR _CNInterrupt(void)
{
    IFS0bits.CNIF = 0;      /* Clear CN interrupt flag */

    /* Don't check bus if its interrupt is disabled. Avoids a race condition */
    if(REQ_CN_BIT == 1)
        checkBus();
}


//readTemp addr 0x9E

int main()
{
    byte i;
    long l;
    ADPCFG = 0xFFFF;

    initI2C();
    initBus();

    _TRISB0 = TRIS_IN;
    _TRISB1 = TRIS_IN;


    #ifdef HAS_UART
    initInterruptUarts();
    #endif


    for(i=0; i<16; i++)
        cfgRegs[i] = 65;

    for(i=0; i<TEMP_DATA_SIZE; i++)
        tempData[i] = 0;


    while(1)
    {
        /* Give it a second */
        for(l=0; l<10000; l++);

        /* Need a way of detecting failures here. A timer module would do. */

        byte rx = readTemp(0x90);

        /* Read error */
        if(rx == 255)
            initI2C();

        tempData[0] = rx;


        rx = readTemp(0x6E);
        if(rx == 255) initI2C();
        tempData[1] = rx;

        rx = readTemp(0xEA);
        if(rx == 255) initI2C();
        tempData[2] = rx;

        rx = readTemp(0xEE);
        if(rx == 255) initI2C();
        tempData[3] = rx;

        rx = readTemp(0xE8);
        if(rx == 255) initI2C();
        tempData[4] = rx;
    }
}
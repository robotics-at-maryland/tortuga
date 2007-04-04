#include <p30fxxxx.h>
#include <string.h>

//_FOSC( CSW_FSCM_OFF & FRC );
_FOSC( CSW_FSCM_OFF & EC_PLL8 );
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
 * Req = C13
 * Akn = C14
 * RW  = C15
 */

/* Bus pin assignments */
#define REQ_CN_BIT  (CNEN1bits.CN1IE)
#define IN_REQ      _RC13
#define TRIS_REQ    _TRISC13

#define LAT_AKN     _LATC14
#define TRIS_AKN    _TRISC14

#define IN_RW       _RE8
#define TRIS_RW     _TRISE8

#define RW_READ     0
#define RW_WRITE    1

#define BUS_CMD_PING            0
#define BUS_CMD_ID              1
#define BUS_CMD_READ_REG        2
#define BUS_CMD_WRITE_REG       3
#define BUS_CMD_MARKER1         4
#define BUS_CMD_DEPTH           5
#define BUS_CMD_LCD_WRITE       6
#define BUS_CMD_LCD_REFRESH     7
#define BUS_CMD_LCD_LIGHT_ON    8
#define BUS_CMD_LCD_LIGHT_OFF   9


/* Transmit buffer */
#define TXBUF_LEN 30
byte txBuf[TXBUF_LEN];
byte txPtr = 0;

/* Misc LCD pins */
#define LAT_E   _LATB4
#define LAT_RS  _LATB5
#define LAT_BL  _LATC15

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
#define STATE_WRITE_LCD     3

byte busState = 0;
byte nParam = 0;
byte p1=0, p2=0;



byte lcdBuf[32];
byte lcdUpdate = 0;

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
                    txBuf[0] = sprintf(txBuf+1, "I am LCD PIC.");
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

                case BUS_CMD_LCD_WRITE:
                {
                    busState = STATE_WRITE_LCD;
                    nParam = 0;
                    break;
                }

                case BUS_CMD_LCD_REFRESH:
                {
                    lcdUpdate++;
                    break;
                }

                case BUS_CMD_LCD_LIGHT_OFF:
                {
                    LAT_BL = 0;
                    break;
                }

                case BUS_CMD_LCD_LIGHT_ON:
                {
                    LAT_BL = 1;
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

            if(nParam == 1)
                p2 = data;

            nParam++;

            if(nParam == 2)
            {
                nParam=0;
                busState = STATE_TOP_LEVEL;
                cfgRegs[p1] = data;
            }
            break;
        }


        case STATE_WRITE_LCD:
        {
            if(nParam == 0)
                p1 = data;

            if(nParam == 1)
                p2 = data;

            nParam++;

            if(nParam == 2)
            {
                nParam=0;
                busState = STATE_TOP_LEVEL;
                lcdBuf[p1] = data;
            }
            break;
        }

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
    IPC3bits.CNIP = 6;      /* Raise CN interrupt priority above ADC */
    IFS0bits.CNIF = 0;      /* Clear CN interrupt flag */
    IEC0bits.CNIE = 1;      /* Turn on CN interrupts */
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





void lcdPulse()
{
    long i;
    LAT_E = 1;
    for(i=0; i<2500; i++);
    LAT_E = 0;
    for(i=0; i<2500; i++);
}


void lcdWrite(byte b)
{
    LATB = (LATB & 0xFFF0) | (b&0x0F);
}


void lcdByte(byte b)
{
    lcdWrite( (b & 0xF0) >> 4);
    lcdPulse();
    lcdWrite(b & 0x0F);
    lcdPulse();
}

void initLCD()
{
    long i=0;
    ADPCFG = 0xFFFF;
    LATB = 0;
    TRISB = 0;


    lcdWrite (0x00);
    for(i=0; i<100000; i++);
    LAT_RS = 0;
    lcdWrite(0x03);   /* init with specific nibbles to start 4-bit mode */

    lcdPulse();
    lcdPulse();
    lcdPulse();

    lcdWrite(0x02);
    lcdPulse();
    lcdByte(0x2C);    /* function set (all lines, 5x7 characters) */

    //lcdByte(0x0C);    /* display ON, cursor off, no blink */
    lcdByte(0x0F);

    lcdByte(0x01);    /* clear display */
    lcdByte(0x06);    /* entry mode set, increment & scroll left */
    LAT_RS = 1;
}

void lcdCmd(byte b)
{
    LAT_RS = 0;
    lcdByte(b);
    LAT_RS = 1;
}

void lcdChar(byte b)
{
    lcdByte(b);
}


void main()
{
    byte i;

    _TRISC15 = TRIS_OUT;
    _LATC15 = 0;

    for(i=0; i<16; i++)
        cfgRegs[i] = 65;

    initBus();

    initLCD();
    byte data1[] = "Controller Init ";
    byte data2[] = "LCD Buffer Empty";

    for(i=0; i<16; i++)
    {
        lcdChar(data1[i]);
        lcdBuf[i] = data2[i];
        lcdBuf[i+16] = data2[i];
    }

    while(1)
    {
        while(lcdUpdate > 0)
        {
            /* Dump LCD buffer to display */
            lcdCmd(0x80);

            for(i=0; i<16; i++)
                lcdChar(lcdBuf[i]);

            lcdCmd(0xC0);
            for(i=0; i<16; i++)
                lcdChar(lcdBuf[i+16]);

            lcdUpdate--;
        }
    }
}

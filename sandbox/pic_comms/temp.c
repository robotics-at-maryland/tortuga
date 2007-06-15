/***************
* Parts of the I2C code in this program came from a Microchip Application Note.
* License says we can use it with Microchip products.
***************/


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
 * RW  = E8 again
 */

/* Bus pin assignments */
#define REQ_CN_BIT  (CNEN1bits.CN1IE)
#define WATER_CN_BIT  (CNEN1bits.CN2IE)
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
#define BUS_CMD_THRUSTERS_ON    10
#define BUS_CMD_THRUSTERS_OFF   11
#define BUS_CMD_MARKER2         12
#define BUS_CMD_CHECKWATER      14
#define BUS_CMD_TEMP            15
#define BUS_CMD_BOARDSTATUS     16
#define BUS_CMD_HARDKILL        17

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


#define I2C_TIMEOUT 100000

byte busState = 0;
byte nParam = 0;
byte p1=0, p2=0;


byte AckI2C(void)
{
    I2CCONbits.ACKDT = 0;          //Set for ACk
    I2CCONbits.ACKEN = 1;

    long timeout = 0;
    while(I2CCONbits.ACKEN)
        if(timeout++ == I2C_TIMEOUT)
            return 255;
    return 0;
}


unsigned int initI2C(void)
{
    //This function will initialize the I2C(1) peripheral.
    //First set the I2C(1) BRG Baud Rate.

    TRISFbits.TRISF2 = 1;
    TRISFbits.TRISF3 = 1;

    /* Turn i2c off */
    I2CCONbits.I2CEN = 0;

    //Consult the dSPIC Data Sheet for information on how to calculate the
    //Baud Rate.

   // I2CBRG = 0x004f;
    I2CBRG = 0x054f;

    //Now we will initialise the I2C peripheral for Master Mode, No Slew Rate
    //Control, and leave the peripheral switched off.

    I2CCON = 0x1200;

    I2CRCV = 0x0000;
    I2CTRN = 0x0000;
    //Now we can enable the peripheral

    I2CCON = 0x9200;
}

unsigned int getI2C(void)
{
    I2CCONbits.RCEN = 1;           //Enable Master receive
    Nop();

    long timeout = 0;
    while(!I2CSTATbits.RBF)
    {
        if(timeout++ == I2C_TIMEOUT)
            return 255;
    }
    return(I2CRCV);                //Return data in buffer
}


byte StartI2C(void)
{
    //This function generates an I2C start condition and returns status
    //of the Start.
    long timeout=0;
    I2CCONbits.SEN = 1;        //Generate Start COndition
    while(I2CCONbits.SEN)
        if(timeout++ == I2C_TIMEOUT)
            return 255;

    return 0;

    //return(I2C1STATbits.S);   //Optionally return status
}

unsigned int RestartI2C(void)
{
    //This function generates an I2C Restart condition and returns status
    //of the Restart.
    long timeout=0;
    I2CCONbits.RSEN = 1;       //Generate Restart

    while(I2CCONbits.RSEN)
        if(timeout++ == I2C_TIMEOUT)
            return 255;

    return 0;

    //return(I2C1STATbits.S);   //Optional - return status
}

unsigned int StopI2C(void)
{
    //This function generates an I2C stop condition and returns status
    //of the Stop.

    I2CCONbits.PEN = 1;        //Generate Stop Condition
    long timeout=0;
    while(I2CCONbits.PEN)
        if(timeout++ == I2C_TIMEOUT)
            return 255;
    return 0;

    //return(I2C1STATbits.P);   //Optional - return status
}

unsigned int WriteI2C(unsigned char b)
{
    //This function transmits the byte passed to the function
    //while (I2C1STATbits.TRSTAT);  //Wait for bus to be idle
    I2CTRN = b;                 //Load byte to I2C1 Transmit buffer
    long timeout=0;
    while(I2CSTATbits.TBF)
        if(timeout++ == I2C_TIMEOUT)
            return 255;
    return 0;
}

unsigned int IdleI2C(void)
{
    long timeout=0;
    while(I2CSTATbits.TRSTAT)
    {
        if(timeout++ == I2C_TIMEOUT)
        {
            return 255;
        }
    }
    return 0;
}

unsigned int getsI2C(unsigned char *rdptr, unsigned char Length)
{
    while (Length --)
    {
        *rdptr++ = getI2C();        //get a single byte

        if(I2CSTATbits.BCL)        //Test for Bus collision
        {
            return(-1);
        }

        if(Length)
        {
            AckI2C();               //Acknowledge until all read
        }
    }
    return(0);
}

long readTemp(byte addr)
{
    long temp=0;
    byte rb[2];
    // 1001111

    IdleI2C();                  //wait for bus Idle
    StartI2C();                 //Generate Start condition
    WriteI2C(addr | 0x01);   //Write 1 byte - R/W bit should be 1 for read
    IdleI2C();                  //wait for bus Idle
    getsI2C(rb, 2);

    temp = rb[0]; // * 10 + (1 * rb[1])/16;;

    AckI2C();
    StopI2C();
    return temp;
}



/* Average depth, as computed by ADC ISR */
long avgDepth = 0;

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
                    txBuf[0] = sprintf(txBuf+1, "I am temp/power/water/start PIC.");
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

                case BUS_CMD_BOARDSTATUS:
                {
                    txBuf[0] = 1;
                    txBuf[1] = PORTB & 0x7F;
                    break;
                }

                case BUS_CMD_DEPTH:
                {
                    txBuf[0] = 2;   /* Depth is 2 bytes */
                    txBuf[1] = (avgDepth & 0xFF00) >> 8;
                    txBuf[2] = avgDepth & 0xFF;
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

                case BUS_CMD_HARDKILL:
                {
                    _LATC15 = 1; /* Uh oh.... master kill */
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

            if(nParam == 1)
                p2 = data;

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
 * Drop the first marker. I am assuming we have multiple markers. This is
 * really here to let me play with interrupts and learn how to use the
 * timer module. I cannot occupy the slave while the marker drops, so
 * marker command sets marker output to 1, and then a timer interrupt must
 * bring it back to 0.
 */
void dropMarker(byte id)
{
    /* Set appropriate output to 1 */
    if(id == 0)
        _LATB1 = 1;
    else
        _LATB2 = 1;


    /* Timer1 is a Type A timer. Evidently there are other types
     * The clock rate is 96MHz, after PLL. So.. it seems that:
     * (1/96e6) * (256 prescaler) * (4 clocks per insn) * (65536 period) = 0.69 seconds.
     * Oh well, 2.79 seconds of soleniod operation should be enough time to drop a
     * marker, but I would like to know the reason for this discrepantcy.
     */

    PR1 = 65535;            /* Period */
    TMR1 = 0;               /* Reset timer */
    IFS0bits.T1IF = 0;      /* Clear interrupt flag */
    IEC0bits.T1IE = 1;      /* Enable interrupts */
    T1CONbits.TCS = 0;      /* Use internal clock */
    T1CONbits.TCKPS = 3;    /* 1:256 prescaler */
    T1CONbits.TON = 1;      /* Start Timer1 */
}

/* ISR for Timer1. Used for turning off marker soleniod after it was turned on */
void _ISR _T1Interrupt(void)
{
    IFS0bits.T1IF = 0;      /* Clear interrupt flag */
    IEC0bits.T1IE = 0;      /* Disable interrupts */

    /* This timer kills both solenoids. If one marker is dropped, and another is
     * dropped before the first soleniod deactivates, the timer is reset and both
     * solenids will deactivate when the timer expires.
     */

    _LATB1 = 0;         /* Turn off marker soleniod (or LED in my case) */
    _LATB2 = 0;         /* Turn off marker soleniod (or LED in my case) */

    T1CONbits.TON = 0;  /* Stop Timer1 */
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

void enableWaterInterrupt()
{
    WATER_CN_BIT = 1; /* Turn on CN for the pin */
    checkBus();
}

void disableWaterInterrupt()
{
    WATER_CN_BIT = 0;    /* Turn off CN for the pin */
}


/* Initialize the CN interrupt to watch the Req line */
void initCN()
{
    enableBusInterrupt();
    enableWaterInterrupt();
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

    if(WATER_CN_BIT == 1 && _RB0 == 1)  /* WATER!!! */
    {
        _RC15 = 1;      /* Hard Kill */
    }

    /* Don't check bus if its interrupt is disabled. Avoids a race condition */
    if(REQ_CN_BIT == 1)
        checkBus();
}


int depthArray[100];
int dp=0;


//readTemp addr 0x9E

void main()
{
    byte i;
    long l;
    _TRISB0 = TRIS_IN;  /* Water sensor  */
    _TRISB1 = TRIS_IN;  /* Start Switch  */
    _TRISB2 = TRIS_IN;  /* Power board 1 */
    _TRISB3 = TRIS_IN;  /* Power board 2 */
    _TRISB4 = TRIS_IN;  /* Power board 3 */
    _TRISB5 = TRIS_IN;  /* Power board 4 */
    _TRISB6 = TRIS_IN;  /* Power board 4 */

    _LATC15 = 0;
    _TRISC15 = TRIS_OUT; /* Hard Kill */

    for(i=0; i<16; i++)
        cfgRegs[i] = 65;

    for(i=0; i<TEMP_DATA_SIZE; i++)
        tempData[i] = 0;

    initI2C();

    /* Temp pic does not need ADC code, but keep this around if we want to combine the two */
 //   initADC();
    initBus();
    ADPCFG = 0xFFFF;

    while(1)
    {
        /* Give it a second */
        for(l=0; l<10000; l++);

        /* Need a way of detecting failures here. A timer module would do. */

        byte rx = readTemp(0x9E);

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
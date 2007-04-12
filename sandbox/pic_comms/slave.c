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

/* Transmit buffer */
#define TXBUF_LEN 60
byte txBuf[TXBUF_LEN];
byte txPtr = 0;



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
byte p1=0, p2=0;

/* Average depth, as computed by ADC ISR */
long avgDepth = 0;

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
                    txBuf[0] = sprintf(txBuf+1, "I am depth/water/marker/thruster PIC.");
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

                case BUS_CMD_MARKER1:
                {
                    dropMarker(0);
                    break;
                }

                case BUS_CMD_MARKER2:
                {
                    dropMarker(1);
                    break;
                }

                case BUS_CMD_THRUSTERS_ON:
                {
                    _LATB3 = 1;
                    break;
                }

                case BUS_CMD_THRUSTERS_OFF:
                {
                    _LATB3 = 0;
                    break;
                }

                case BUS_CMD_CHECKWATER:
                {
                    txBuf[0] = 1;
                    txBuf[1] = _RB4;
                    break;
                }

                case BUS_CMD_DEPTH:
                {
                    txBuf[0] = 2;   /* Depth is 2 bytes */
                    txBuf[1] = (avgDepth & 0xFF00) >> 8;
                    txBuf[2] = avgDepth & 0xFF;
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


int depthArray[100];
int dp=0;

void _ISR _ADCInterrupt(void)
{
    IFS0bits.ADIF = 0;
    byte i=0;

    long ad=0;


    depthArray[dp++] = ADCBUF0;
    if(dp >= 100)
        dp=0;

    ad = 0;
    for(i=0; i<100; i++)
        ad+= depthArray[i];

    ad /= 100;


    /*
     * Why does disabling and re-enabling the CN interrupts muck up the data transfers?
     * Maybe some interrupt bits need to be dealt with. Since average depth is only a 16-bit
     * value, the assignment operation is atomic and there should be no data race here.
     */
    disableBusInterrupt();
    avgDepth = ad;
    enableBusInterrupt();
}

/*
 * Initialize ADC for depth sensor. All this code really needs to be split up
 * into different files, each one different for each slave. But for now, write
 * and test everything in one file.
 */
void initADC()
{
    avgDepth = 0x1234;
    ADPCFG = 0xFFFF;
    ADPCFGbits.PCFG0 = 0;
    _TRISB0 = TRIS_IN;

    ADCON1 = 0x0000;
    ADCON1bits.SSRC = 7;    /* Conversion starts when sampling ends */
    ADCON1bits.ASAM = 1;    /* Automatic sampling enabled */

    ADCON1bits.FORM = 0;    /* Plain format */

    ADCHS = 0x0000;
    ADCSSL = 0;
    ADCON3bits.SAMC=0x1F;

    ADCON3bits.ADCS = 4;        /* ADC needs so much time to convert at 30 MIPS */
    ADCON2bits.SMPI = 0x0F;  /* Interrupt every 16 samples - why not? */
          //Clear the A/D interrupt flag bit
    IFS0bits.ADIF = 0;

        //Set the A/D interrupt enable bit
    IEC0bits.ADIE = 1;

    ADCON1bits.ADON = 1;
    ADCON1bits.SAMP = 1;    /* Start auto-sampling */
}

void main()
{
    byte i;

    _TRISB1 = TRIS_OUT; /* Marker 1 */
    _TRISB2 = TRIS_OUT; /* Marker 2 */
    _TRISB3 = TRIS_OUT; /* Thruster Safety */
    _TRISB4 = TRIS_IN;  /* Water sensor */

    for(i=0; i<16; i++)
        cfgRegs[i] = 65;


    initADC();
    initBus();

    while(1);
}
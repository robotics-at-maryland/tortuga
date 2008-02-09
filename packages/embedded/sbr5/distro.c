#include <p30fxxxx.h>
#include <string.h>
#include "buscodes.h"

#define SENSORBOARD_DISTROBOARD
#include "uart.c"
#include "i2c.c"

//_FOSC( CSW_FSCM_OFF & FRC );
_FOSC( CSW_FSCM_OFF & ECIO); //EC_PLL4); //ECIO );
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
#define REQ_CN_BIT  (CNEN1bits.CN9IE)
#define IN_REQ      _RG7
#define TRIS_REQ    _TRISG7

#define LAT_AKN     _LATD8
#define TRIS_AKN    _TRISD8

#define IN_RW       _RD9
#define TRIS_RW     _TRISD9

#define RW_READ     0
#define RW_WRITE    1


/* Level specification for battery inputs */
#define BATT_ON     1

/* Battery input pin assignments */
#define IN_BATT1    _LATC1
#define TRIS_BATT1  _TRISC1

#define IN_BATT2    _LATC2
#define TRIS_BATT2  _TRISC2

#define IN_BATT3    _LATC13
#define TRIS_BATT3  _TRISC13

#define IN_BATT4    _LATC14
#define TRIS_BATT4  _TRISC14


/* We know this one is active low */
#define IN_WTRSEN   _LATG6
#define TRIS_WTRSEN _TRISG6
#define WATER_CN_BIT  (CNEN1bits.CN8IE)


/* Level specification for marker outputs */
#define MRKR_ON     1

#define LAT_MRKR1   _LATF0
#define TRIS_MRKR1  _TRISF0

#define LAT_MRKR2   _LATF1
#define TRIS_MRKR2  _TRISF1


/* Level specification for thruster enables */
#define MOTR_ON     1

/* Thruster pin assignments */
#define LAT_MOTR1   _LATG0
#define TRIS_MOTR1  _TRISG0

#define LAT_MOTR2   _LATG1
#define TRIS_MOTR2  _TRISG1

#define LAT_MOTR3   _LATG8
#define TRIS_MOTR3  _TRISG8

#define LAT_MOTR4   _LATG9
#define TRIS_MOTR4  _TRISG9

#define LAT_MOTR5   _LATG12
#define TRIS_MOTR5  _TRISG12

#define LAT_MOTR6   _LATG13
#define TRIS_MOTR6  _TRISG13


/* Power kill output level specification */
#define PWRKILL_ON  1

/* Power kill pin assignment */
#define LAT_PWRKILL _LATG14
#define TRIS_PWRKILL _TRISG14


/* Kill switch level specification */
#define KILLSW_ON 1

/* Kill switch input */
#define IN_KILLSW   _RG15
#define TRIS_KILLSW _TRISG15



/* Transmit buffer */
#define TXBUF_LEN 60
byte txBuf[TXBUF_LEN];
byte txPtr = 0;


void dropMarker(byte id);


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
#define STATE_SETSPEED_U1   4
#define STATE_SETSPEED_U2   5

byte busState = 0;
byte nParam = 0;
byte p1=0;

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
                    txBuf[0] = sprintf(txBuf+1, "PWR MRK THR");
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

#ifdef HAS_U1
                case BUS_CMD_GETREPLY_U1:
                {
                    txBuf[0] = 1;
                    if(U1CanRead())
                        txBuf[1] = U1ReadByte();
                    else
                        txBuf[1] = 0xFF;
                    break;
                }
#endif

#ifdef HAS_U2
                case BUS_CMD_GETREPLY_U2:
                {
                    txBuf[0] = 1;
                    if(U2CanRead())
                        txBuf[1] = U2ReadByte();
                    else
                        txBuf[1] = 0xFF;
                    break;
                }
#endif

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


                case BUS_CMD_THRUSTER1_OFF:  { LAT_MOTR1 = ~MOTR_ON; break; }
                case BUS_CMD_THRUSTER2_OFF:  { LAT_MOTR2 = ~MOTR_ON; break; }
                case BUS_CMD_THRUSTER3_OFF:  { LAT_MOTR3 = ~MOTR_ON; break; }
                case BUS_CMD_THRUSTER4_OFF:  { LAT_MOTR4 = ~MOTR_ON; break; }
                case BUS_CMD_THRUSTER5_OFF:  { LAT_MOTR5 = ~MOTR_ON; break; }
                case BUS_CMD_THRUSTER6_OFF:  { LAT_MOTR6 = ~MOTR_ON; break; }

                case BUS_CMD_THRUSTER1_ON:  { LAT_MOTR1 = MOTR_ON; break; }
                case BUS_CMD_THRUSTER2_ON:  { LAT_MOTR2 = MOTR_ON; break; }
                case BUS_CMD_THRUSTER3_ON:  { LAT_MOTR3 = MOTR_ON; break; }
                case BUS_CMD_THRUSTER4_ON:  { LAT_MOTR4 = MOTR_ON; break; }
                case BUS_CMD_THRUSTER5_ON:  { LAT_MOTR5 = MOTR_ON; break; }
                case BUS_CMD_THRUSTER6_ON:  { LAT_MOTR6 = MOTR_ON; break; }

                case BUS_CMD_THRUSTER_STATE:
                {
                    disableBusInterrupt();
                    txBuf[0] = 1;
                    txBuf[1] = 0;

                    if(LAT_MOTR1 == MOTR_ON) txBuf[1] |= 0x01;
                    if(LAT_MOTR2 == MOTR_ON) txBuf[1] |= 0x02;
                    if(LAT_MOTR3 == MOTR_ON) txBuf[1] |= 0x04;
                    if(LAT_MOTR4 == MOTR_ON) txBuf[1] |= 0x08;
                    if(LAT_MOTR5 == MOTR_ON) txBuf[1] |= 0x10;
                    if(LAT_MOTR6 == MOTR_ON) txBuf[1] |= 0x20;
                    enableBusInterrupt();
                    break;
                }

                case BUS_CMD_BOARDSTATUS:
                {
                    disableBusInterrupt();
                    txBuf[0] = 1;
                    txBuf[1] = 0;

                    if(IN_WTRSEN == 0) txBuf[1] |= 0x01;
                    if(IN_KILLSW == KILLSW_ON) txBuf[1] |= 0x02;

                    if(IN_BATT4 == BATT_ON) txBuf[1] |= 0x04;
                    if(IN_BATT3 == BATT_ON) txBuf[1] |= 0x08;
                    if(IN_BATT2 == BATT_ON) txBuf[1] |= 0x10;
                    if(IN_BATT1 == BATT_ON) txBuf[1] |= 0x20;
                    enableBusInterrupt();
                    break;
                }

                case BUS_CMD_HARDKILL:
                {
                    LAT_PWRKILL = PWRKILL_ON; /* Uh oh.... master kill */
                    break;
                }
            }
        }
        break;


#ifdef HAS_UART
        case STATE_SETSPEED_U1:
        case STATE_SETSPEED_U2:
        {
            if(nParam == 0)
                p1 = data;

            nParam++;

            if(nParam == 2)
            {
                nParam=0;
                UARTSendSpeed((busState == STATE_SETSPEED_U1) ? U1_MM_ADDR : U2_MM_ADDR,
                              p1, data, (busState == STATE_SETSPEED_U1) ? 0 : 1);
                busState = STATE_TOP_LEVEL;
            }
            break;
        }
#endif


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
    return (PORTD & 0x00FF);
}


/* Take bus out of high-impedance state and write a byte there */
void writeBus(byte b)
{
    TRISD = TRISD & 0xFF00;
    LATD = (LATD & 0xFF00) | b;
}


/* Put bus in high-impedance state. */
void freeBus()
{
    TRISD = TRISD | 0x00FF;
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
        LAT_MRKR1 = MRKR_ON;
    else
        LAT_MRKR2 = MRKR_ON;


    /* Timer1 is a Type A timer. Evidently there are other types
     * The clock rate is 96MHz, after PLL. So.. it seems that:
     * (1/96e6) * (256 prescaler) * (4 clocks per insn) * (65536 period) = 0.69 seconds.
     * Oh well, 2.79 seconds of soleniod operation should be enough time to drop a
     * marker, but I would like to know the reason for this discrepantcy.
     */

    PR1 = 7500;            /* Period */
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

    LAT_MRKR1 = ~MRKR_ON;         /* Turn off marker soleniod (or LED in my case) */
    LAT_MRKR2 = ~MRKR_ON;         /* Turn off marker soleniod (or LED in my case) */

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
    IPC2bits.U1TXIP = 6;    /* TX at priority 6 */
    IPC2bits.U1RXIP = 5;    /* RX at priority 5 */
    IPC3bits.CNIP = 4;      /* Bus at priority 4 */
    IPC2bits.ADIP = 2;      /* ADC at priority 2 */

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

    if(WATER_CN_BIT == 1 && IN_WTRSEN == 0)  /* WATER!!! */
    {
        LAT_PWRKILL = PWRKILL_ON;      /* Hard Kill */
    }

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
//     disableBusInterrupt();
    avgDepth = ad;
//     enableBusInterrupt();
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
    long l;

    TRIS_BATT1 = TRIS_IN;
    TRIS_BATT2 = TRIS_IN;
    TRIS_BATT3 = TRIS_IN;
    TRIS_BATT4 = TRIS_IN;

    TRIS_WTRSEN = TRIS_IN;

    LAT_MRKR1 = ~MRKR_ON;
    LAT_MRKR2 = ~MRKR_ON;

    TRIS_MRKR1 = TRIS_OUT;
    TRIS_MRKR2 = TRIS_OUT;

    LAT_MOTR1 = ~MOTR_ON;
    LAT_MOTR2 = ~MOTR_ON;
    LAT_MOTR3 = ~MOTR_ON;
    LAT_MOTR4 = ~MOTR_ON;
    LAT_MOTR5 = ~MOTR_ON;
    LAT_MOTR6 = ~MOTR_ON;

    TRIS_MOTR1 = TRIS_OUT;
    TRIS_MOTR2 = TRIS_OUT;
    TRIS_MOTR3 = TRIS_OUT;
    TRIS_MOTR4 = TRIS_OUT;
    TRIS_MOTR5 = TRIS_OUT;
    TRIS_MOTR6 = TRIS_OUT;

    LAT_PWRKILL = ~PWRKILL_ON;
    TRIS_PWRKILL = TRIS_OUT;

    TRIS_KILLSW = TRIS_IN;

    initBus();
    initADC();
    initI2C();

#ifdef HAS_UART
    initInterruptUarts();
#endif


    for(i=0; i<16; i++)
        cfgRegs[i] = 65;


    while(1)
    {
        /* Give it a second */
        for(l=0; l<10000; l++);

        byte rx = readTemp(0x90);

        /* Read error */
        if(rx == 255)
            initI2C();

        /* Do something with the temperature here */

    }
}
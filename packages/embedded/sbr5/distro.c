#include <p30fxxxx.h>
#include <string.h>
#include "buscodes.h"

#define SENSORBOARD_DISTROBOARD
#include "uart.c"
#include "i2c.c"

//_FOSC( CSW_FSCM_OFF & FRC );
_FOSC( CSW_FSCM_OFF & HS); //EC_PLL4); //ECIO );
//_FOSC( FRC_LO_RANGE);
//_FOSCSEL(FRC);
//_FPOR( PWRT_OFF);
//Turn off the watchdog
_FWDT ( WDT_OFF );

#define TRIS_OUT 0
#define TRIS_IN  1
#define byte unsigned char

/* Bus pin assignments */
#define REQ_CN_BIT  (CNEN2bits.CN22IE)
#define IN_REQ      _RA6
#define TRIS_REQ    _TRISA6

#define LAT_AKN     _LATF0
#define TRIS_AKN    _TRISF0

#define IN_RW       _RF1
#define TRIS_RW     _TRISF1

#define RW_READ     0
#define RW_WRITE    1


/* Level specification for marker outputs */
#define MRKR_ON     1

#define LAT_MRKR1   _LATC1
#define TRIS_MRKR1  _TRISC1

#define LAT_MRKR2   _LATG15
#define TRIS_MRKR2  _TRISG15


/* Level specification for thruster enables */
#define MOTR_ON     1

/* Thruster pin assignments */
#define LAT_MOTR1   _LATG8
#define TRIS_MOTR1  _TRISG8

#define LAT_MOTR2   _LATG7
#define TRIS_MOTR2  _TRISG7

#define LAT_MOTR3   _LATG6
#define TRIS_MOTR3  _TRISG6

#define LAT_MOTR4   _LATC4
#define TRIS_MOTR4  _TRISC4

#define LAT_MOTR5   _LATC3
#define TRIS_MOTR5  _TRISC3

#define LAT_MOTR6   _LATC2
#define TRIS_MOTR6  _TRISC2


/* Kill switch level specification */
#define KILLSW_ON 0

/* Kill switch input */
#define IN_KILLSW   _RD15
#define TRIS_KILLSW _TRISD15


/* LED Bar and Fan output definitions */
/* Level specification */
#define BAR_ON  1

#define LAT_BAR1    _LATF7
#define TRIS_BAR1   _TRISF7

#define LAT_BAR2    _LATF6
#define TRIS_BAR2   _TRISF6

#define LAT_BAR3    _LATA14
#define TRIS_BAR3   _TRISA14

#define LAT_BAR4    _LATA15
#define TRIS_BAR4   _TRISA15

#define LAT_BAR5    _LATD8
#define TRIS_BAR5   _TRISD8

#define LAT_BAR6    _LATD9
#define TRIS_BAR6   _TRISD9

#define LAT_BAR7    _LATD10
#define TRIS_BAR7   _TRISD10

#define LAT_BAR8    _LATD11
#define TRIS_BAR8   _TRISD11


/* Regulator board enables */
#define LAT_12V_EN  _LATC13
#define TRIS_12V_EN _TRISC13

#define LAT_5V_EN  _LATC14
#define TRIS_5V_EN _TRISC14

/* Regulator board level specification */
/* Who knows? */
#define REG_ON  0


/* LED level specification */
#define LED_ON          1


/* LED pin definitions */
#define LAT_LED_STA1    _LATF8
#define TRIS_LED_STA1   _TRISF8

#define LAT_LED_STA2    _LATD14
#define TRIS_LED_STA2   _TRISD14

#define LAT_LED_ERR     _LATA10
#define TRIS_LED_ERR    _TRISA10

#define LAT_LED_OVR     _LATA9
#define TRIS_LED_OVR    _TRISA9

/* Motor controller and marker currents */
#define ADC_IM1         0x09
#define ADC_IM2         0x08
#define ADC_IM3         0x07
#define ADC_IM4         0x06
#define ADC_IM5         0x02
#define ADC_IM6         0x03
#define ADC_IM7         0x04
#define ADC_IM8         0x05

/* The reference input. Hasn't been attached yet */
#define ADC_VREF        0x0A

/* 5V and 12V voltage and current sensing */
#define ADC_V5V         0x0B
#define ADC_V12V        0x0C

#define ADC_I5V         0x0E
#define ADC_I12V        0x0F

#define ADC_IAUX        0x0D


unsigned int iMotor[8];
unsigned int refVoltage;
unsigned int v5VBus;
unsigned int v12VBus;
unsigned int i5VBus;
unsigned int i12VBus;
unsigned int iAux;


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


byte myTemperature = 255;

byte ovrReg = 0;    /* Overcurrent register */


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


                case BUS_CMD_THRUSTER1_OFF:  { LAT_MOTR1 = ~MOTR_ON; ovrReg &= ~0x01; break; }
                case BUS_CMD_THRUSTER2_OFF:  { LAT_MOTR2 = ~MOTR_ON; ovrReg &= ~0x02; break; }
                case BUS_CMD_THRUSTER3_OFF:  { LAT_MOTR3 = ~MOTR_ON; ovrReg &= ~0x04; break; }
                case BUS_CMD_THRUSTER4_OFF:  { LAT_MOTR4 = ~MOTR_ON; ovrReg &= ~0x08; break; }
                case BUS_CMD_THRUSTER5_OFF:  { LAT_MOTR5 = ~MOTR_ON; ovrReg &= ~0x10; break; }
                case BUS_CMD_THRUSTER6_OFF:  { LAT_MOTR6 = ~MOTR_ON; ovrReg &= ~0x20; break; }

                case BUS_CMD_THRUSTER1_ON:  { if(!(ovrReg & 0x01)) LAT_MOTR1 = MOTR_ON; break; }
                case BUS_CMD_THRUSTER2_ON:  { if(!(ovrReg & 0x02)) LAT_MOTR2 = MOTR_ON; break; }
                case BUS_CMD_THRUSTER3_ON:  { if(!(ovrReg & 0x04)) LAT_MOTR3 = MOTR_ON; break; }
                case BUS_CMD_THRUSTER4_ON:  { if(!(ovrReg & 0x08)) LAT_MOTR4 = MOTR_ON; break; }
                case BUS_CMD_THRUSTER5_ON:  { if(!(ovrReg & 0x10)) LAT_MOTR5 = MOTR_ON; break; }
                case BUS_CMD_THRUSTER6_ON:  { if(!(ovrReg & 0x20)) LAT_MOTR6 = MOTR_ON; break; }

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

                case BUS_CMD_BAR1_OFF: { LAT_BAR1 = ~BAR_ON; break; }
                case BUS_CMD_BAR2_OFF: { LAT_BAR2 = ~BAR_ON; break; }
                case BUS_CMD_BAR3_OFF: { LAT_BAR3 = ~BAR_ON; break; }
                case BUS_CMD_BAR4_OFF: { LAT_BAR4 = ~BAR_ON; break; }
                case BUS_CMD_BAR5_OFF: { LAT_BAR5 = ~BAR_ON; break; }
                case BUS_CMD_BAR6_OFF: { LAT_BAR6 = ~BAR_ON; break; }
                case BUS_CMD_BAR7_OFF: { LAT_BAR7 = ~BAR_ON; break; }
                case BUS_CMD_BAR8_OFF: { LAT_BAR8 = ~BAR_ON; break; }

                case BUS_CMD_BAR1_ON: { LAT_BAR1 = BAR_ON; break; }
                case BUS_CMD_BAR2_ON: { LAT_BAR2 = BAR_ON; break; }
                case BUS_CMD_BAR3_ON: { LAT_BAR3 = BAR_ON; break; }
                case BUS_CMD_BAR4_ON: { LAT_BAR4 = BAR_ON; break; }
                case BUS_CMD_BAR5_ON: { LAT_BAR5 = BAR_ON; break; }
                case BUS_CMD_BAR6_ON: { LAT_BAR6 = BAR_ON; break; }
                case BUS_CMD_BAR7_ON: { LAT_BAR7 = BAR_ON; break; }
                case BUS_CMD_BAR8_ON: { LAT_BAR8 = BAR_ON; break; }

                case BUS_CMD_BAR_STATE:
                {
                    disableBusInterrupt();
                    txBuf[0] = 1;
                    txBuf[1] = 0;

                    if(LAT_BAR1 == BAR_ON) txBuf[1] |= 0x01;
                    if(LAT_BAR2 == BAR_ON) txBuf[1] |= 0x02;
                    if(LAT_BAR3 == BAR_ON) txBuf[1] |= 0x04;
                    if(LAT_BAR4 == BAR_ON) txBuf[1] |= 0x08;
                    if(LAT_BAR5 == BAR_ON) txBuf[1] |= 0x10;
                    if(LAT_BAR6 == BAR_ON) txBuf[1] |= 0x20;
                    if(LAT_BAR7 == BAR_ON) txBuf[1] |= 0x40;
                    if(LAT_BAR8 == BAR_ON) txBuf[1] |= 0x80;

                    enableBusInterrupt();
                    break;
                }


                case BUS_CMD_BOARDSTATUS:
                {
                    disableBusInterrupt();
                    txBuf[0] = 1;
                    txBuf[1] = 0;

                    if(IN_KILLSW == KILLSW_ON)
                        txBuf[1] = 0x01;

                    enableBusInterrupt();
                    break;
                }


                case BUS_CMD_TEMP:
                {
                    txBuf[0] = 1;
                    txBuf[1] = myTemperature;
                    break;
                }

                case BUS_CMD_READ_OVR:
                {
                    txBuf[0] = 1;
                    txBuf[1] = ovrReg;
                    break;
                }

                case BUS_CMD_READ_IMOTOR:
                {
                    byte i=0;
                    txBuf[0] = 16;

                    /* 16 bytes. 1 - 16 */
                    /* Motor controller currents. Big-endian. */
                    for(i=0; i<8; i++)
                    {
                        txBuf[2*i+1] = iMotor[i] >> 8;
                        txBuf[2*i+2] = iMotor[i] & 0xFF;
                    }
                    break;
                }

                case BUS_CMD_READ_ASTATUS:
                {
                    txBuf[0] = 12;
                    txBuf[1] = refVoltage >> 8;
                    txBuf[2] = refVoltage & 0xFF;

                    txBuf[3] = v5VBus >> 8;
                    txBuf[4] = v5VBus & 0xFF;

                    txBuf[5] = i5VBus >> 8;
                    txBuf[6] = i5VBus & 0xFF;

                    txBuf[7] = v12VBus >> 8;
                    txBuf[8] = v12VBus & 0xFF;

                    txBuf[9] = i12VBus >> 8;
                    txBuf[10] = i12VBus & 0xFF;

                    txBuf[11] = iAux >> 8;
                    txBuf[12] = iAux & 0xFF;
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


/* Initialize the CN interrupt to watch the Req line */
void initCN()
{
    enableBusInterrupt();
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
//     avgDepth = ad;
//     enableBusInterrupt();
}

/*
 * Initialize ADC for depth sensor. All this code really needs to be split up
 * into different files, each one different for each slave. But for now, write
 * and test everything in one file.
 */
void initADC()
{
    ADPCFG = 0x0000;        /* Well damn. All analog. */
    TRISB = 0xFFFF;         /* All inputs */

    ADCON1 = 0x0000;
    ADCON1bits.SSRC = 7;    /* Conversion starts when sampling ends */
    ADCON1bits.ASAM = 0;    /* Automatic sampling disabled */

    ADCON1bits.FORM = 0;    /* Plain format */

    ADCON2bits.VCFG = 0x00; /* AVDD and AVSS as reference */

    ADCHS = 0x0000;
    ADCSSL = 0;
    ADCON3bits.SAMC=0x1F;   /* Sample for a long time. We have decent impedances */

    ADCON3bits.ADCS = 8;    /* ADC needs so much time to convert at 30 MIPS */
    ADCON2bits.SMPI = 0x0F; /* Interrupt every 16 samples - why not? We dont use interrupts here */

    ADCON2bits.BUFM = 0;    /* No buffering */
    ADCON2bits.ALTS = 0;    /* No alternating. Bad 440 memories. */

    IFS0bits.ADIF = 0;      /* Clear interrupt flag */
    IEC0bits.ADIE = 0;      /* No interrupts please */

    ADCON1bits.ADON = 1;
    ADCON1bits.SAMP = 0;    /* Start auto-sampling */
}

void setADC(byte inPort)
{
    byte t;
    ADCHS = inPort;
    for(t=0; t<10; t++);
}

unsigned int readADC()
{
    byte t;
    ADCON1bits.SAMP = 1;
    for(t=0; t<2; t++);
    ADCON1bits.SAMP = 0;
    while (!ADCON1bits.DONE);
    return ADCBUF0;
}

void main()
{
    byte i;
    long l;
    ovrReg = 0; /* Clear overcurrent register */

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

    TRIS_KILLSW = TRIS_IN;

    LAT_BAR1 = ~BAR_ON;
    LAT_BAR2 = ~BAR_ON;
    LAT_BAR3 = ~BAR_ON;
    LAT_BAR4 = ~BAR_ON;
    LAT_BAR5 = ~BAR_ON;
    LAT_BAR6 = ~BAR_ON;
    LAT_BAR7 = ~BAR_ON;
    LAT_BAR8 = ~BAR_ON;

    TRIS_BAR1 = TRIS_OUT;
    TRIS_BAR2 = TRIS_OUT;
    TRIS_BAR3 = TRIS_OUT;
    TRIS_BAR4 = TRIS_OUT;
    TRIS_BAR5 = TRIS_OUT;
    TRIS_BAR6 = TRIS_OUT;
    TRIS_BAR7 = TRIS_OUT;
    TRIS_BAR8 = TRIS_OUT;


    TRIS_LED_STA1 = TRIS_OUT;
    TRIS_LED_STA2 = TRIS_OUT;
    TRIS_LED_ERR = TRIS_OUT;
    TRIS_LED_OVR = TRIS_OUT;

    LAT_LED_STA1 = LED_ON;
    LAT_LED_STA2 = ~LED_ON;
    LAT_LED_ERR = ~LED_ON;
    LAT_LED_OVR = ~LED_ON;

    LAT_5V_EN = REG_ON;
    LAT_12V_EN = REG_ON;

    TRIS_5V_EN = TRIS_OUT;
    TRIS_12V_EN = TRIS_OUT;

    initBus();
    initADC();
    initI2C();

#ifdef HAS_UART
    initInterruptUarts();
#endif


    byte motorADCs[]=
    {
        ADC_IM1, ADC_IM2, ADC_IM3, ADC_IM4,
        ADC_IM5, ADC_IM6, ADC_IM7, ADC_IM8
    };


    for(i=0; i<16; i++)
        cfgRegs[i] = 65;

    byte i2cErrCount = 0;

    while(1)
    {
        /* Give it a second */
        for(l=0; l<10000; l++);

        byte rx = readTemp(0x90);

        /* Read error */
        if(rx == 255)
        {
            if(i2cErrCount < 10)
                i2cErrCount++;
            else
                myTemperature = 255;

            initI2C();
        } else
        {
            i2cErrCount = 0;
            myTemperature = rx;
        }

        for(i=0; i<8; i++)
        {
            setADC(motorADCs[i]);
            iMotor[i] = readADC();
        }

        setADC(ADC_VREF);
        refVoltage = readADC();

        setADC(ADC_V5V);
        v5VBus = readADC();

        setADC(ADC_I5V);
        i5VBus = readADC();


        setADC(ADC_V12V);
        v12VBus = readADC();

        setADC(ADC_I12V);
        i12VBus = readADC();

        setADC(ADC_V5V);
        v5VBus = readADC();

        setADC(ADC_IAUX);
        iAux = readADC();
    }
}
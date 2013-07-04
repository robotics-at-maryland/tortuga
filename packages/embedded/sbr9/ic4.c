#include <p30fxxxx.h>
#include <stdio.h>
#include <string.h>
#include "buscodes.h"

#define SENSORBOARD_IC4
#include "uart.c"

//_FOSC( CSW_FSCM_OFF & FRC );
_FOSC( CSW_FSCM_OFF & ECIO & PWRT_64); //EC_PLL4); //ECIO );
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
#define REQ_CN_BIT  (CNEN1bits.CN2IE)
#define IN_REQ      _RB0
#define TRIS_REQ    _TRISB0

#define LAT_AKN     _LATD8
#define TRIS_AKN    _TRISD8

#define IN_RW       _RD9
#define TRIS_RW     _TRISD9

//kanga
#define TRIS_CAM_REL _TRISF4
#define TRIS_CAM_LED _TRISF5
#define LAT_CAM_REL _LATF4
#define LAT_CAM_LED _LATF5

#define RW_READ     0
#define RW_WRITE    1


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
                    txBuf[0] = sprintf(txBuf+1, "Depth");
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
                //kanga - 7/3/2013
                case BUS_CMD_CAM_RELAY_ON:
                {
                    LAT_CAM_REL = 1;
                    LAT_CAM_LED = 1;
                    break;
                }
                case BUS_CMD_CAM_RELAY_OFF:
                {
                    LAT_CAM_REL = 0;
                    LAT_CAM_LED = 0;
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
    return (PORTD & 0x0F) | ((PORTB & 0x0F00) >> 4);
}


/* Take bus out of high-impedance state and write a byte there */
void writeBus(byte b)
{
    TRISD = TRISD & 0xFFF0;
    TRISB = TRISB & 0xF0FF;

    LATD = (LATD & 0xFFF0) | (b & 0x0F);
    LATB = (LATB & 0xF0FF) | ((b & 0xF0) << 4);
}


/* Put bus in high-impedance state. */
void freeBus()
{
    TRISB = TRISB | 0x0F00;
    TRISD = TRISD | 0x0F;
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
    IPC2bits.U1TXIP = 6;    /* TX at priority 6 */
    IPC2bits.U1RXIP = 5;    /* RX at priority 5 */

    IPC6bits.U2TXIP = 6;    /* TX at priority 6 */
    IPC6bits.U2RXIP = 5;    /* RX at priority 5 */


    IPC3bits.CNIP = 4;      /* Bus at priority 4 */
    IPC2bits.ADIP = 2;      /* ADC at priority 2 */

    IFS0bits.CNIF = 0;      /* Clear CN interrupt flag */
    IEC0bits.CNIE = 1;      /* Turn on CN interrupts */
}

//kanga 7/2/2013
void initCam()
{
/* Init Camera cycling state to on*/
    TRIS_CAM_REL = TRIS_OUT;
    TRIS_CAM_LED = TRIS_OUT;
    LAT_CAM_REL = 1;
    LAT_CAM_LED = 1;
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
    initCam();
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

#define NUM_SAMPLES 128

int depthArray[NUM_SAMPLES];
int dp= 0;

void _ISR _ADCInterrupt(void)
{
    IFS0bits.ADIF= 0;
    byte i= 0;
    long long ad= 0;

    depthArray[dp++]= ADCBUF0;
    if(dp >= NUM_SAMPLES)
        dp= 0;

    depthArray[dp++]= ADCBUF1;
    if(dp >= NUM_SAMPLES)
        dp= 0;

    depthArray[dp++]= ADCBUF2;
    if(dp >= NUM_SAMPLES)
        dp= 0;

    depthArray[dp++]= ADCBUF3;
    if(dp >= NUM_SAMPLES)
        dp= 0;

    depthArray[dp++]= ADCBUF4;
    if(dp >= NUM_SAMPLES)
        dp= 0;

    depthArray[dp++]= ADCBUF5;
    if(dp >= NUM_SAMPLES)
        dp= 0;

    depthArray[dp++]= ADCBUF6;
    if(dp >= NUM_SAMPLES)
        dp= 0;

    depthArray[dp++]= ADCBUF7;
    if(dp >= NUM_SAMPLES)
        dp= 0;

    depthArray[dp++]= ADCBUF8;
    if(dp >= NUM_SAMPLES)
        dp= 0;

    depthArray[dp++]= ADCBUF9;
    if(dp >= NUM_SAMPLES)
        dp= 0;

    depthArray[dp++]= ADCBUFA;
    if(dp >= NUM_SAMPLES)
        dp= 0;

    depthArray[dp++]= ADCBUFB;
    if(dp >= NUM_SAMPLES)
        dp= 0;

    depthArray[dp++]= ADCBUFC;
    if(dp >= NUM_SAMPLES)
        dp= 0;

    depthArray[dp++]= ADCBUFD;
    if(dp >= NUM_SAMPLES)
        dp= 0;

    depthArray[dp++]= ADCBUFE;
    if(dp >= NUM_SAMPLES)
        dp= 0;

    depthArray[dp++]= ADCBUFF;
    if(dp >= NUM_SAMPLES)
        dp= 0;

    ad= 0;
    for(i= 0; i < NUM_SAMPLES;i++)
        ad+= depthArray[i];

    ad= (ad / NUM_SAMPLES);

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
    ADPCFGbits.PCFG1 = 0;
    _TRISB0 = TRIS_IN;

    ADCON1 = 0x0000;
    ADCON1bits.SSRC = 7;    /* Conversion starts when sampling ends */
    ADCON1bits.ASAM = 1;    /* Automatic sampling enabled */

    ADCON1bits.FORM = 0;    /* Plain format */

    ADCHS = 0x0001;         /* Convert pin AN1 */
    ADCSSL = 0;
    ADCON3bits.SAMC=0x1F;   /* Sample time = 15Tad */

    ADCON3bits.ADCS= 0x3F;   /* Max out the ADC sampling time */
    ADCON2bits.SMPI = 0x0F;  /* Interrupt every 16 samples - why not? */
    //ADCON2bits.SMPI= 0x00;   /* because you're sampling too fast */

    //Clear the A/D interrupt flag bit
    IFS0bits.ADIF = 0;

    T2CONbits.TON= 0; // Turn off the timer
    //now set up timer 2
    T2CON= 0x0020;   /* bits 5-4: 2->x64 prescaler */
    TMR2= 0x0000;    /* Clear timer */
    PR2= 390;        /* 10MHz/(4) = 2.5MHz = Fcy
                        Fcy/64 = 39,062.5 Hz = Freq-post-prescalar = Fpps
                        10ms * Fpps = 390.625 = PR2
                      */
    _T2IF= 0;        /* Clear interrupt flag */
    T2CONbits.TON= 1;

        //Set the A/D interrupt enable bit
    IEC0bits.ADIE = 1;

    ADCON1bits.ADON = 1;
    ADCON1bits.SAMP = 1;    /* Start auto-sampling */
    T2CONbits.TON= 1;       /* Start timer */
}

int main()
{
    byte i;
    initBus();

    for(i=0; i<16; i++)
        cfgRegs[i] = 65;

    initADC();
    
//kanga - "it wasn't my fault. nick told me to do it"
//initInterruptUarts();
    
    while(1)
    {
        if (ADCON1bits.DONE && _T2IF)
        {
            _T2IF=0;
            ADCON1bits.SAMP= 1;
        }


        //any more main loop code here
    }

    return 0;
}

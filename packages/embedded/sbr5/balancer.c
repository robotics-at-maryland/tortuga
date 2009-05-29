// #define BBR2
#define BBR3



#include <p30fxxxx.h>
#include <string.h>
#include "buscodes.h"

#define SENSORBOARD_BALANCERBOARD
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

/* I Originally had plans to have BBR2 and BBR3 have different
 * numbers of batteries, but then I realized that both needed
 * to have 6 in order to have the same number of packets
 */
#define BATT_COUNT 6

/*
 * Bus = D1 D0 E5-E0
 * Req = C13
 * Akn = C14
 * RW  = E8 again
 */

/* Bus pin assignments */
#define REQ_CN_BIT  (CNEN2bits.CN22IE)
#define IN_REQ      _RA6
#define TRIS_REQ    _TRISA6

#define LAT_AKN     _LATA7
#define TRIS_AKN    _TRISA7

#define IN_RW       _RG14
#define TRIS_RW     _TRISG14

#define RW_READ     0
#define RW_WRITE    1




/* Level specification for battery inputs */
#define BATT_ON     1

/* Battery input pin assignments */
#define IN_BATT1    _RG7
#define TRIS_BATT1  _TRISG7

#define IN_BATT2    _RC4
#define TRIS_BATT2  _TRISC4

#define IN_BATT3    _RC2
#define TRIS_BATT3  _TRISC2

#define IN_BATT4    _RG15
#define TRIS_BATT4  _TRISG15

#define IN_BATT5    _RG12
#define TRIS_BATT5  _TRISG12


#ifdef BBR3
    /* Rev 3 only */
    #define IN_BATT6    _RG1
    #define TRIS_BATT6  _TRISG1
#endif

#ifdef BBR2
    /* We know this one is active low */
    #define IN_WTRSEN   _RB15
    #define TRIS_WTRSEN _TRISB15
    #define WATER_CN_BIT  (CNEN1bits.CN12IE)
#endif

#ifdef BBR3
    /* We know this one is active low */
    #define IN_WTRSEN   _RB2
    #define TRIS_WTRSEN _TRISB2
    #define WATER_CN_BIT  (CNEN1bits.CN4IE)
#endif

/* Power kill output level specification */
#define PWRKILL_ON  0

#ifdef BBR2
    /* Power kill pin assignment */
    #define LAT_PWRKILL _LATB14
    #define TRIS_PWRKILL _TRISB14
#endif

#ifdef BBR3
    /* Power kill pin assignment */
    #define LAT_PWRKILL _LATD14
    #define TRIS_PWRKILL _TRISD14
#endif


/* Level specification for battery control outputs */
#define BATT_ENABLE 0


/* Battery control outputs */
#define LAT_BATT1_CTL   _LATG8
#define TRIS_BATT1_CTL  _TRISG8

#define LAT_BATT2_CTL   _LATG6
#define TRIS_BATT2_CTL  _TRISG6

#define LAT_BATT3_CTL   _LATC3
#define TRIS_BATT3_CTL  _TRISC3

#define LAT_BATT4_CTL   _LATC1
#define TRIS_BATT4_CTL  _TRISC1

#ifdef BBR3
    #define LAT_BATT5_CTL   _LATG13
    #define TRIS_BATT5_CTL  _TRISG13

    #define LAT_BATT6_CTL   _LATG0
    #define TRIS_BATT6_CTL  _TRISG0
#else
    #define LAT_BATT5_CTL   _LATG0
    #define TRIS_BATT5_CTL  _TRISG0

    #define LAT_BATT6_CTL   _LATG13
    #define TRIS_BATT6_CTL  _TRISG13
#endif

/* LED level specification */
#define LED_ON          0

/* LED pin definitions */
#define LAT_LED_STA     _LATF7
#define TRIS_LED_STA    _TRISF7

#define LAT_LED_BATTLOW     _LATF8
#define TRIS_LED_BATTLOW    _TRISF8


/* ADC Inputs */
#define ADC_B1V     0x0B
#define ADC_B2V     0x09
#define ADC_B3V     0x07
#define ADC_B4V     0x03

#ifdef BBR3
    #define ADC_B5V     0x05
    #define ADC_B6V     0x0F
#else
    #define ADC_B5V     0x0F
    #define ADC_B6V     0x05
#endif

#define ADC_26V     0x0D

#define ADC_B1I     0x0C
#define ADC_B2I     0x0A
#define ADC_B3I     0x08
#define ADC_B4I     0x06
#define ADC_B5I     0x04

#ifdef BBR3
    #define ADC_B5I     0x04
    #define ADC_B6I     0x0E
#else
    #define ADC_B5I     0x0E
    #define ADC_B6I     0x04
#endif

unsigned int vBatt[7];
unsigned int iBatt[6];

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
                    txBuf[0] = sprintf(txBuf+1, "BAL WTR PWR");
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


                case BUS_CMD_BOARDSTATUS:
                {
//                     disableBusInterrupt();
                    txBuf[0] = 1;
                    txBuf[1] = 0;

                    if(IN_BATT6 == BATT_ON) txBuf[1] |= 0x20;
#ifdef BBR3
                    if(IN_BATT5 == BATT_ON) txBuf[1] |= 0x10;
#endif
                    if(IN_BATT4 == BATT_ON) txBuf[1] |= 0x08;
                    if(IN_BATT3 == BATT_ON) txBuf[1] |= 0x04;
                    if(IN_BATT2 == BATT_ON) txBuf[1] |= 0x02;
                    if(IN_BATT1 == BATT_ON) txBuf[1] |= 0x01;
                    if(IN_WTRSEN == 0) txBuf[1] |= 0x80;

//                     enableBusInterrupt();
                    break;
                }

                case BUS_CMD_TEMP:
                {
                    txBuf[0] = 1;
                    txBuf[1] = myTemperature;
                    break;
                }

                case BUS_CMD_BATT1_OFF: {LAT_BATT1_CTL = ~BATT_ENABLE; break;}
                case BUS_CMD_BATT2_OFF: {LAT_BATT2_CTL = ~BATT_ENABLE; break;}
                case BUS_CMD_BATT3_OFF: {LAT_BATT3_CTL = ~BATT_ENABLE; break;}
                case BUS_CMD_BATT4_OFF: {LAT_BATT4_CTL = ~BATT_ENABLE; break;}
                case BUS_CMD_BATT5_OFF: {
#ifdef BBR3
                                          LAT_BATT5_CTL = ~BATT_ENABLE;
#endif
                                          break;
                                        }
                case BUS_CMD_BATT6_OFF: {LAT_BATT6_CTL = ~BATT_ENABLE; break;}

                case BUS_CMD_BATT1_ON: {LAT_BATT1_CTL = BATT_ENABLE; break;}
                case BUS_CMD_BATT2_ON: {LAT_BATT2_CTL = BATT_ENABLE; break;}
                case BUS_CMD_BATT3_ON: {LAT_BATT3_CTL = BATT_ENABLE; break;}
                case BUS_CMD_BATT4_ON: {LAT_BATT4_CTL = BATT_ENABLE; break;}
                case BUS_CMD_BATT5_ON: {
#ifdef BBR3
                                         LAT_BATT5_CTL = BATT_ENABLE;
#endif
                                         break;
                                       }
                case BUS_CMD_BATT6_ON: {LAT_BATT6_CTL = BATT_ENABLE; break;}

                case BUS_CMD_EXTPOWER:
                {
                    LAT_BATT6_CTL = BATT_ENABLE;
#ifdef BBR3
                    LAT_BATT5_CTL = ~BATT_ENABLE;
#endif
                    LAT_BATT1_CTL = ~BATT_ENABLE;
                    LAT_BATT2_CTL = ~BATT_ENABLE;
                    LAT_BATT3_CTL = ~BATT_ENABLE;
                    LAT_BATT4_CTL = ~BATT_ENABLE;
                   break;
                }

                case BUS_CMD_INTPOWER:
                {
                    LAT_BATT1_CTL = BATT_ENABLE;
                    LAT_BATT2_CTL = BATT_ENABLE;
                    LAT_BATT3_CTL = BATT_ENABLE;
                    LAT_BATT4_CTL = BATT_ENABLE;
#ifdef BBR3
                    LAT_BATT5_CTL = BATT_ENABLE;
#endif
                    LAT_BATT6_CTL = ~BATT_ENABLE;
                    break;
                }


                case BUS_CMD_HARDKILL:
                {
                    LAT_PWRKILL = PWRKILL_ON; /* Uh oh.... master kill */
                    break;
                }

                case BUS_CMD_BATTSTATE:
                {
                    txBuf[0] = 1;
                    txBuf[1] = 0;
                    if(LAT_BATT6_CTL == BATT_ENABLE) txBuf[1] |= 0x20;
#ifdef BBR3
                    if(LAT_BATT5_CTL == BATT_ENABLE) txBuf[1] |= 0x10;
#endif
                    if(LAT_BATT4_CTL == BATT_ENABLE) txBuf[1] |= 0x08;
                    if(LAT_BATT3_CTL == BATT_ENABLE) txBuf[1] |= 0x04;
                    if(LAT_BATT2_CTL == BATT_ENABLE) txBuf[1] |= 0x02;
                    if(LAT_BATT1_CTL == BATT_ENABLE) txBuf[1] |= 0x01;
                    break;
                }

                case BUS_CMD_BATTVOLTAGE:
                {
                    txBuf[0] = 12;
                    byte i;

                    /* Battery voltages. Big-endian. */
                    for(i=0; i < BATT_COUNT + 1; i++)
                    {
                        unsigned int t = vBatt[i];
                        txBuf[2*i+1] = t >> 8;
                        txBuf[2*i+2] = t & 0xFF;
                    }
                    break;
                }

                case BUS_CMD_BATTCURRENT:
                {
                    txBuf[0] = 10;
                    byte i;

                    /* Battery currents. Big-endian. */
                    for(i=0; i < BATT_COUNT; i++)
                    {
                        unsigned int t = iBatt[i];
                        txBuf[2*i+1] = t >> 8;
                        txBuf[2*i+2] = t & 0xFF;
                    }
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
//     enableWaterInterrupt();
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
    {
        checkBus();
        actLight();
    }
}



void actLight()
{
    PR2 = 100;            /* Period */
    TMR2 = 0;               /* Reset timer */
    IFS0bits.T2IF = 0;      /* Clear interrupt flag */
    IEC0bits.T2IE = 1;      /* Enable interrupts */
    T2CONbits.TCS = 0;      /* Use internal clock */
    T2CONbits.TCKPS = 3;    /* 1:256 prescaler */
    T2CONbits.TON = 1;      /* Start Timer2 */
    LAT_LED_STA = LED_ON;
}

/* ISR for Timer2. Used for making the ACT light pretty */
void _ISR _T2Interrupt(void)
{
    IFS0bits.T2IF = 0;      /* Clear interrupt flag */
    IEC0bits.T2IE = 0;      /* Disable interrupts */
    LAT_LED_STA = ~LED_ON;
    T2CONbits.TON = 0;  /* Stop Timer1 */
}


/*
 * Initialize ADC for depth sensor. All this code really needs to be split up
 * into different files, each one different for each slave. But for now, write
 * and test everything in one file.
 */
void initADC()
{

#ifdef BBR2
    ADPCFG = 0x8000;
#endif

#ifdef BBR3
    ADPCFG = 0x8000;
#endif

    ADCON1 = 0x0000; // SAMP bit = 0 ends sampling ...
    // and starts converting
    ADCHS = 0x0002; // Connect RB2/AN2 as CH0 input ..
    // in this example RB2/AN2 is the input
    ADCSSL = 0;
    ADCON3 = 0x0002; // Manual Sample, Tad = internal 2 Tcy
    ADCON2 = 0;
    ADCON1bits.ADON = 1; // turn ADC ON

    return;

    ADPCFG = 0x0000;        /* Well damn. All analog. */
    TRISB = 0xFFFF;         /* All inputs */

    ADCON1 = 0x0000;
    ADCON2 = 0x0000;
    ADCON3 = 0x0000;

    ADCSSL = 0;
    ADCON3 = 0x1F02;

    IFS0bits.ADIF = 0;      /* Clear interrupt flag */
    IEC0bits.ADIE = 0;      /* No interrupts please */

    ADCON1bits.ADON = 1;
    return;
}

void setADC(byte inPort)
{
    byte t;
//     ADCON1bits.ADON = 0;
     ADCHS = inPort;
//     ADCON1bits.ADON = 1;
    for(t=0; t<10; t++);
}

long readADC()
{
    long l;
    ADCON1bits.SAMP = 1; // start sampling ...
    for(l=0; l<100; l++);
    ADCON1bits.SAMP = 0; // start Converting
    while (!ADCON1bits.DONE); // conversion done?

    return ADCBUF0;
}

#define CAL_I5V_A 7.305594
#define CAL_I5V_B -3065.893302

#define CAL_I12V_A 7.463615
#define CAL_I12V_B -3098.165312

#define CAL_V_A 11.334405
#define CAL_V_B 1.527331

unsigned int applyCalibration(unsigned int x, float a, float b)
{
    float t = x * a + b;
    return t;
}


const static byte iADCs[6]=
{
    ADC_B1I, ADC_B2I, ADC_B3I, ADC_B4I, ADC_B5I, ADC_B6I
};


#define IHISTORY_SIZE   32
#define IHISTORY_LOG2   5

unsigned int iADCVal[6][IHISTORY_SIZE];

unsigned int avgRow(byte r)
{
    unsigned long long t = 0;
    byte i;
    for(i=0; i<IHISTORY_SIZE; i++)
        t += iADCVal[r][i];
    return t >> IHISTORY_LOG2;
}


void initBattlowLight()
{
    PR3 = 100;              /* Period */
    TMR3 = 0;               /* Reset timer */
    IFS0bits.T3IF = 0;      /* Clear interrupt flag */
    IEC0bits.T3IE = 1;      /* Enable interrupts */
    T3CONbits.TCS = 0;      /* Use internal clock */
    T3CONbits.TCKPS = 3;    /* 1:256 prescaler */
    T3CONbits.TON = 1;      /* Start Timer3 */
}

byte battlowState = 0;  /* 0 = off, 1 = on */
byte battlowEnabled = 1;

int battlowOffTime()
{
    long vb = 0;
#ifdef BBR2
    byte nBatt = 0;
    if(IN_BATT6 == BATT_ON) {vb += vBatt[5]; nBatt++;}
    /* There should never be a BATT5 in BBR2, so skip it */
    if(IN_BATT4 == BATT_ON) {vb += vBatt[3]; nBatt++;}
    if(IN_BATT3 == BATT_ON) {vb += vBatt[2]; nBatt++;}
    if(IN_BATT2 == BATT_ON) {vb += vBatt[1]; nBatt++;}
    if(IN_BATT1 == BATT_ON) {vb += vBatt[0]; nBatt++;}

    if(nBatt == 0)
        vb = 0;
    else
        vb /= nBatt;
#else
    vb= vBatt[6]; /* We don't have to average anything!
                     Just use the ADC measurement of the
                     26V rail */
#endif

    vb -= 24500;
    if(vb <= 0)
        vb = 1;

    vb *= 3;

    if(vb > 9000)
    {
        battlowEnabled = 0;
        return 50;
    } else
        battlowEnabled = 1;

    return vb;
}


/* ISR for Timer3. Used for making the battery low indicator pretty */
void _ISR _T3Interrupt(void)
{
    IFS0bits.T3IF = 0;      /* Clear interrupt flag */
    LAT_LED_BATTLOW = ~LED_ON;

    if(battlowState == 0) /* Light was off */
    {
        battlowState = 1;

        if(battlowEnabled == 1)
            LAT_LED_BATTLOW = LED_ON;

        PR3 = 200;  /* Turn light on for X much */
    } else
    {
        battlowState = 0;
        LAT_LED_BATTLOW = ~LED_ON;
        PR3 = battlowOffTime();  /* Turn light off for X much */
    }
}




void main()
{
    byte writeIndex = 0;
    byte i;
    long l;
    TRISB = 0xFFFF;         /* All inputs */

    LAT_BATT1_CTL = BATT_ENABLE;
    LAT_BATT2_CTL = BATT_ENABLE;
    LAT_BATT3_CTL = BATT_ENABLE;
    LAT_BATT4_CTL = BATT_ENABLE;
#ifdef BBR2
    LAT_BATT5_CTL = ~BATT_ENABLE;
#else
    LAT_BATT5_CTL = BATT_ENABLE;
#endif
    LAT_BATT6_CTL = BATT_ENABLE;

    TRIS_BATT1_CTL = TRIS_OUT;
    TRIS_BATT2_CTL = TRIS_OUT;
    TRIS_BATT3_CTL = TRIS_OUT;
    TRIS_BATT4_CTL = TRIS_OUT;
#ifdef BBR2
    TRIS_BATT5_CTL = TRIS_IN;
#else
    TRIS_BATT5_CTL = TRIS_OUT;
#endif
    TRIS_BATT6_CTL = TRIS_OUT;

    TRIS_BATT1 = TRIS_IN;
    TRIS_BATT2 = TRIS_IN;
    TRIS_BATT3 = TRIS_IN;
    TRIS_BATT4 = TRIS_IN;
    TRIS_BATT5 = TRIS_IN;
    TRIS_BATT6 = TRIS_IN;

    TRIS_WTRSEN = TRIS_IN;

    LAT_PWRKILL = ~PWRKILL_ON;
    TRIS_PWRKILL = TRIS_OUT;

    TRIS_LED_STA = TRIS_OUT;
    TRIS_LED_BATTLOW = TRIS_OUT;

    LAT_LED_STA = LED_ON;
    LAT_LED_BATTLOW = LED_ON;


    initBus();
    LAT_LED_STA = LED_ON;
    LAT_LED_BATTLOW = ~LED_ON;

  //  while(1);

    initADC();
    initI2C();

    initBattlowLight();

#ifdef HAS_UART
    initInterruptUarts();
#endif

    for(l=0; l<50000; l++);

    LAT_LED_STA = LED_ON;
    LAT_LED_BATTLOW = ~LED_ON;

    for(l=0; l<50000; l++);

//     LAT_PWRKILL = PWRKILL_ON;

    LAT_LED_STA = ~LED_ON;

    for(i=0; i<16; i++)
        cfgRegs[i] = 65;


    byte i2cErrCount = 0;

    while(1)
    {
//         checkBus();
        /* Give it a second */
//        for(l=0; l<10000; l++);

        byte rx = readTemp(0x9E);

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

        static const byte vADCs[]={ADC_B1V, ADC_B2V, ADC_B3V, ADC_B4V, ADC_B5V, ADC_B6V, ADC_26V};

        /* Measure battery voltages */
        for(i=0; i < 7; i++) {
#ifdef BBR2
            /* There is no fifth battery in BBR2, so skip it */
            if(i == 4) { vBatt[4]= 0; continue; }
#endif
            setADC(vADCs[i]);
            vBatt[i] = applyCalibration(readADC(), CAL_V_A, CAL_V_B);
        }

        /* Maintain running averages of the I sensors */
        for(i=0; i < BATT_COUNT; i++) {
#ifdef BBR2
            /* There is no fifth battery in BBR2, so skip it */
            if(i == 4) { iADCVal[4][writeIndex] = 0; continue; }
#endif
            setADC(iADCs[i]);
            iADCVal[i][writeIndex] = readADC();
        }
        writeIndex++;

        if(writeIndex >= IHISTORY_SIZE)
            writeIndex = 0;

        /* Calculate running averages of the battery currents */
        for(i=0; i < BATT_COUNT; i++) {
#ifdef BBR2
            /* There is no fifth battery in BBR2, so skip it. */
            if(i == 4) { iBatt[4] = 0; continue; }
#endif
            iBatt[i] = applyCalibration(avgRow(i), CAL_I12V_A, CAL_I12V_B);
        }
    }
}

/*
    SBR5 = Define this for Sensor Board Revision 5 and 6.
           This is the first generation of the backplane-aware sensor board.
           This board is listed as SensorBoardRev5 in Eagle

    SBR7 = Define this for Sensor Board Revision 7 and 8. This
           This is the second generation of the backplane-aware sensor board.
           Originally two were made.
           This is listed as SensorBoardRev7 in Eagle and was made at Goddard
           only a few days before leaving.
*/

//#define SBR5
#define SBR7
#define MOTORBOARD

#include <p30fxxxx.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "buscodes.h"

#define SENSORBOARD_IC1
#include "uart.c"

_FOSC( CSW_FSCM_OFF & ECIO & PWRT_64);
_FWDT ( WDT_OFF );


#define TRIS_OUT 0
#define TRIS_IN  1
#define byte unsigned char

/*
 * Bus = D1 D0 E5-E0
 * Akn = D2
 * RW  = E8
 */

/* Bus pin assignments */
#define IN_AKN      _RD2
#define LAT_AKN     _LATD2
#define TRIS_AKN    _TRISD2

#define IN_RW       _RE8
#define TRIS_RW     _TRISE8
#define LAT_RW      _LATE8

#define RW_READ     0
#define RW_WRITE    1


#define LED_ON      1

/* LED pin definitions */
#define LAT_LED_ACT     _LATF1
#define TRIS_LED_ACT    _TRISF1


#ifdef SBR5
    #define LAT_LED_ERR     _LATB6
    #define TRIS_LED_ERR    _TRISB6

    #define LAT_IRQ0     _LATB0
    #define LAT_IRQ1     _LATB1
    #define LAT_IRQ2     _LATB2
    #define LAT_IRQ3     _LATB4
    #define LAT_IRQ4     _LATB3
    #define LAT_IRQ5     _LATB7
    #define LAT_IRQ6     _LATB8

    #define USB_PRESENT     1
#else

    #define LAT_LED_ERR     _LATF0
    #define TRIS_LED_ERR    _TRISF0

    #define LAT_IRQ0     _LATB0
    #define LAT_IRQ1     _LATB1
    #define LAT_IRQ2     _LATB2
    #define LAT_IRQ3     _LATB4
    #define LAT_IRQ4     _LATB6
    #define LAT_IRQ5     _LATB7
    #define LAT_IRQ6     _LATB8

    #define USB_PRESENT     0
#endif




/* USB detection */
#define IN_USBDETECT    _RC15
#define TRIS_USBDETECT  _TRISC15

#define IRQ_IC2         2
#define IRQ_IC3         1
#define IRQ_IC4         0
#define IRQ_DISTRO      3
#define IRQ_BALANCER    4
#define IRQ_SONAR       5
#define IRQ_MOTOR       6

#define SLAVE_ID_POWERBOARD IRQ_DISTRO


#define SLAVE_ID_DEPTH      IRQ_IC4
#define SLAVE_ID_THRUSTERS  IRQ_DISTRO
#define SLAVE_ID_MARKERS    IRQ_MOTOR
#define SLAVE_ID_TEMP       IRQ_IC3
#define SLAVE_ID_LCD        IRQ_IC2
#define SLAVE_ID_HARDKILL   IRQ_BALANCER
#define SLAVE_ID_SONAR	    IRQ_SONAR
#define SLAVE_ID_STARTSW    IRQ_IC3
#define SLAVE_ID_KILLSW     IRQ_DISTRO
#define SLAVE_ID_BATTSTAT   IRQ_BALANCER
#define SLAVE_ID_BARS       IRQ_DISTRO
#define SLAVE_ID_IMOTOR     IRQ_DISTRO
#define SLAVE_ID_VLOW       IRQ_DISTRO
#define SLAVE_ID_DVL        IRQ_DISTRO
#define SLAVE_ID_MOTOR      IRQ_MOTOR
#define SLAVE_ID_TORPEDO    IRQ_MOTOR
#define SLAVE_ID_GRABBER    IRQ_MOTOR

#define SLAVE_ID_MM1        IRQ_IC2
#define SLAVE_ID_MM2        IRQ_IC2
#define SLAVE_ID_MM3        IRQ_IC3
#define SLAVE_ID_MM4        IRQ_IC3
#define SLAVE_ID_MM5        IRQ_IC4
#define SLAVE_ID_MM6        IRQ_IC4

#define SLAVE_MM1_WRITE_CMD BUS_CMD_SETSPEED_U1
#define SLAVE_MM2_WRITE_CMD BUS_CMD_SETSPEED_U2
#define SLAVE_MM3_WRITE_CMD BUS_CMD_SETSPEED_U1
#define SLAVE_MM4_WRITE_CMD BUS_CMD_SETSPEED_U2
#define SLAVE_MM5_WRITE_CMD BUS_CMD_SETSPEED_U1
#define SLAVE_MM6_WRITE_CMD BUS_CMD_SETSPEED_U2

#define SLAVE_MM1_READ_CMD  BUS_CMD_GETREPLY_U1
#define SLAVE_MM2_READ_CMD  BUS_CMD_GETREPLY_U2
#define SLAVE_MM3_READ_CMD  BUS_CMD_GETREPLY_U1
#define SLAVE_MM4_READ_CMD  BUS_CMD_GETREPLY_U2
#define SLAVE_MM5_READ_CMD  BUS_CMD_GETREPLY_U1
#define SLAVE_MM6_READ_CMD  BUS_CMD_GETREPLY_U2


/*
 * Bus Constants
 * BUS_TIMEOUT - how many iterations to wait when waiting for AKN to change state
 *
 * BUS_ERROR   - AKN failed to go high when talking to Slave. Most likely indicates a
 *               Slave fault. Can also mean that some Slave is forcibly holding AKN
 *               low, but this is very unlikely to happen.
 *
 * BUS_FAILURE - A Slave is holding the AKN line high, preventing any further bus
 *               operations. This is catastrophic failure and a Reset may be needed.
 *               It may be possible to retry (in case Slave bus code got interrupted
 *               in the middle of an operation, but this is extremely unlikely (and
 *               should be avoided by disabling that interrupt anyway).
 */
#define BUS_TIMEOUT     25000
#define BUS_ERROR       -1
#define BUS_FAILURE     -2

#define DIAG_TIMEOUT     25000


/* How many chips are in the POST list? */
#ifndef MOTORBOARD
    #define NUM_SLAVES  6
#else
    #define NUM_SLAVES  7
#endif


#ifndef SBR5
    #ifndef SBR7
        #error You have to define a board revision- SBR5 or SBR7. See ic1.c
    #endif
#endif

#ifndef MOTORBOARD
    static const unsigned char postList[]={IRQ_IC2, IRQ_IC3, IRQ_IC4, IRQ_DISTRO, IRQ_BALANCER, IRQ_SONAR};
#else
    static const unsigned char postList[]={IRQ_IC2, IRQ_IC3, IRQ_IC4, IRQ_DISTRO, IRQ_BALANCER, IRQ_SONAR, IRQ_MOTOR};
#endif


static const unsigned char hkSafety[]={0xDE, 0xAD, 0xBE, 0xEF, 0x3E};
static const unsigned char tkSafety[]={0xB1, 0xD0, 0x23, 0x7A, 0x69};
static const unsigned char cdSafety[]={0xBA, 0xDB, 0xEE, 0xEF, 0x4A};

signed int waitchar(byte timeout);


byte failsafeTripped = 0;   /* Gets set to 1 */

/* Takes the high and low byte from a 16bit speed packet and scales it
 * to the new 8-bit format. */
byte convertSpeed(byte high, byte low) {
    byte ret;
    short therealchunk= high;
    long multTemp;

    therealchunk<<= 8;
    therealchunk|= low;
    multTemp= therealchunk;

    multTemp*= 102;
    /* We need a bias for negative numbers */
    if(multTemp < 0)
        multTemp+= 0x3FF;
    multTemp>>= 10;

    if(multTemp < 0)
        ret= 0x7F + multTemp;
    else
        ret= 0x80 + multTemp;

    return ret;
}

/* Read byte from bus */
byte readBus()
{
    return (PORTE & 0x3F) | (_RD0 << 6) | (_RD1 << 7);
}


/* Take bus out of high-impedance state and write a byte to it */
void writeBus(byte b)
{
    TRISE = TRISE & 0xFFC0;
    _TRISD1 = TRIS_OUT;
    _TRISD0 = TRIS_OUT;

     LATE = (LATE & 0xFFC0) | (b & 0x3F);
    _LATD0 = (b & 0x40) >> 6;
    _LATD1 = (b & 0x80) >> 7;
}


/* Put bus in high-impedance state */
void freeBus()
{
    _TRISD1 = TRIS_IN;
    _TRISD0 = TRIS_IN;
    TRISE = TRISE | 0x3F;
}

byte diagMsg=1;

void checkFailsafe();

#define ERR_NOUSB   -1

/* Wait for a byte on the serial console */
/* Returns 0-255 for byte read, or -1 if USB error */
/* Timeout is not implemented... but in two years it seems this has been fine */
signed int waitchar(byte timeout)
{
    //long waitTime=0, j;
    byte x;

    U1STAbits.OERR = 0;
    U1STAbits.FERR = 0;
    U1STAbits.PERR = 0;
    U1STAbits.URXDA = 0;

    checkFailsafe();

    /* Spin in while loop until we receive a byte */
    while(U1STAbits.URXDA == 0)
    {
        /* Do we not have USB? */
        if(IN_USBDETECT != USB_PRESENT)
        {
            /* If USB goes away, we give up */
            return ERR_NOUSB;
        }

        /* We need to check the failsafe timeout */
        /* If failsafe tripped, send motor-off commands */
        checkFailsafe();
    }

    /* Byte is unsigned char and X is defined as byte, so any */
    /* values returned will be in the 0-255 range             */
    x = U1RXREG;
    U1STAbits.URXDA = 0;
    return x;
}


/* Initialize bus */
void initBus()
{
    /* Put everything in high-impedance state */
    freeBus();
    TRIS_RW = TRIS_OUT;
    TRIS_AKN = TRIS_IN;
}


/* Set the given Slave's Req line to the given value */
void setReq(byte req, byte val)
{
    if(req == 0)
        LAT_IRQ0 = val;   /* IRQ 0 - B0 */

    if(req == 1)
        LAT_IRQ1 = val;   /* IRQ 1 - B1 */

    if(req == 2)
        LAT_IRQ2 = val;   /* IRQ 2 - B2 */

    if(req == 3)
        LAT_IRQ3 = val;   /* IRQ 3 - B4 */

    if(req == 4)
        LAT_IRQ4 = val;   /* IRQ 4 - B3 */

    if(req == 5)
        LAT_IRQ5 = val;   /* IRQ 5 - B7 */

    if(req == 6)
        LAT_IRQ6 = val;   /* IRQ 6 - B8 */
}


/* Read a byte from a given Slave */
/* Returns BUS_ERROR or BUS_FAILURE on error */
int busReadByte(byte req)
{
    byte data=0;
    long timeout = 0;

    /* Set RW to read */
    LAT_RW = RW_READ;

    /* Raise Req */
    setReq(req, 1);

    /* Wait for AKN to go high */
    /* Need timeout to detect Slave fault */
    while(IN_AKN == 0)
    {
        if(timeout++ == BUS_TIMEOUT)
        {
            setReq(req, 0);
            LAT_LED_ERR = LED_ON;
            return BUS_ERROR;
        }
    }

    /* Read the data */
    data = readBus();

    /* Drop Req */
    setReq(req, 0);

    /* Wait for Slave to release bus */
    timeout=0;
    while(IN_AKN == 1)
    {
        if(timeout++ == BUS_TIMEOUT)
        {
            LAT_LED_ERR = LED_ON;
            return BUS_FAILURE;     /* We're totally screwed */
        }
    }

    LAT_LED_ERR = ~LED_ON;

    return data;
}


/* Write a byte to a given slave */
/* Returns BUS_ERROR or BUS_FAILURE on error */
int busWriteByte(byte data, byte req)
{
    long timeout=0;

    /* Set RW to write */
    LAT_RW = RW_WRITE;

    /* Put the data on the bus */
    writeBus(data);

    /* Raise Req */
    setReq(req, 1);

    /* Wait for AKN to go high */
    /* Need timeout to detect Slave fault */
    while(IN_AKN == 0)
    {
        if(timeout++ == BUS_TIMEOUT)
        {
            setReq(req, 0);
            freeBus();
            LAT_LED_ERR = LED_ON;
            return BUS_ERROR;
        }
    }

    /* Release bus */
    freeBus();

    /* Drop Req */
    setReq(req, 0);

    /* Wait for Slave to release bus */
    timeout=0;
    while(IN_AKN == 1)
    {
        if(timeout++ == BUS_TIMEOUT)
        {
            LAT_LED_ERR = LED_ON;
            return BUS_FAILURE;     /* We're totally screwed */
        }
    }

    LAT_LED_ERR = ~LED_ON;

    return 0;
}

/* Turn on the yellow LED and set a timer to turn it off about 0.1s later */
void actLight()
{
    PR2 = 100;            /* Period */
    TMR2 = 0;               /* Reset timer */
    IFS0bits.T2IF = 0;      /* Clear interrupt flag */
    IEC0bits.T2IE = 1;      /* Enable interrupts */
    T2CONbits.TCS = 0;      /* Use internal clock */
    T2CONbits.TCKPS = 3;    /* 1:256 prescaler */
    T2CONbits.TON = 1;      /* Start Timer2 */
    LAT_LED_ACT = LED_ON;
}

/* ISR for Timer2. Used for making the ACT light pretty */
void _ISR _T2Interrupt(void)
{
    IFS0bits.T2IF = 0;      /* Clear interrupt flag */
    IEC0bits.T2IE = 0;      /* Disable interrupts */
    LAT_LED_ACT = ~LED_ON;
    T2CONbits.TON = 0;      /* Stop Timer2 */
}


void setMotorFailsafe()
{
    failsafeTripped = 0;
    /* 43945 = 3 sec */
    PR1 = 21972;            /* Period */
    TMR1 = 0;               /* Reset timer */
    IFS0bits.T1IF = 0;      /* Clear interrupt flag */
    IEC0bits.T1IE = 1;      /* Enable interrupts */
    T1CONbits.TCS = 0;      /* Use internal clock */
    T1CONbits.TCKPS = 3;    /* 1:256 prescaler */
    T1CONbits.TON = 1;      /* Start Timer1 */
}

/* ISR for Timer1. Used for the motor failsafe */
void _ISR _T1Interrupt(void)
{
    IFS0bits.T1IF = 0;      /* Clear interrupt flag */
    IEC0bits.T1IE = 0;      /* Disable interrupts */
    T1CONbits.TON = 0;  /* Stop Timer1 */
    failsafeTripped = 1;
}


void initMasterUart()
{
    U1MODE = 0x0000;
//    U1BRG = 1;  /* 7 for 230400, 15 for 115200 194 for 9600  AT 30 MIPS*/
    U1BRG = MASTER_U1_BRG;  /* 7 for 115200 at 15 MIPS */
    U1MODEbits.ALTIO = 1;   // Use alternate IO
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;   // Enable transmit
}


/* Send a byte to the serial console */
void sendByte(byte i)
{
    while(U1STAbits.UTXBF);
    while(!U1STAbits.TRMT);
    U1TXREG = i;
    while(U1STAbits.UTXBF);
    while(!U1STAbits.TRMT);
}


/* General purpose bus receive buffer */
byte rxBuf[60];


/*
 * Read data from bus into rxBuf and return number of bytes read.
 * Returns BUS_ERROR or BUS_FAILURE on error
 */
int readDataBlock(byte req)
{
    int rxPtr, rxLen, rxData;
    rxBuf[0]=0;
    rxLen = busReadByte(req);

    if(rxLen < 0)
        return rxLen;

    for(rxPtr=0; rxPtr<rxLen; rxPtr++)
    {
        rxData = busReadByte(req);

        if(rxData < 0)
            return rxData;

        rxBuf[rxPtr] = rxData;
    }

    rxBuf[rxLen]=0;
    return rxLen;
}


void showString(unsigned char str[], int line)
{
    int i;
    for(i=0; i<str[i]!=0; i++)
    {
        busWriteByte(BUS_CMD_LCD_WRITE, SLAVE_ID_LCD);
        busWriteByte(line*16+i, SLAVE_ID_LCD);
        busWriteByte(str[i], SLAVE_ID_LCD);
    }
    busWriteByte(BUS_CMD_LCD_REFRESH, SLAVE_ID_LCD);
}

int pollStartSw()
{
    /* Read start switch from another chip......... */
    if(busWriteByte(BUS_CMD_STARTSW, SLAVE_ID_STARTSW) != 0)
    {
        sendByte(HOST_REPLY_FAILURE);
        return -1;
    }

    if(readDataBlock(SLAVE_ID_STARTSW) != 1)
    {
        sendByte(HOST_REPLY_FAILURE);
        return -1;
    }

    return rxBuf[0] & 0x01;
}



void checkChip(unsigned char * str, byte irq)
{
    int ret;
    ret = busWriteByte(BUS_CMD_PING, irq);
    if(ret == BUS_ERROR)
        memcpy(str, "fail", 4);

    if(ret == BUS_FAILURE)
        memcpy(str, "EPIC", 4);

    if(ret == 0)
    {
        if(readDataBlock(irq) == 0)
            memcpy(str, " OK ", 4);
        else
            memcpy(str, "rbad", 4);
    }
}


void showBootDiag(int mode)
{
    unsigned char tmp[20];
    if(mode == 1)
    {
        sprintf(tmp, "BALN  DIST  SONR"); //Keith: Changed from "DIST  BALN  SONR" to "BALN  DIST..." (2-17-2011)
        showString(tmp, 0);
        sprintf(tmp, "                ");

        checkChip(tmp, IRQ_BALANCER);
        checkChip(tmp+6, IRQ_DISTRO);
        checkChip(tmp+12, IRQ_SONAR);
        showString(tmp, 1);
    }

    if(mode == 0)
    {
        if(busWriteByte(BUS_CMD_DEPTH, SLAVE_ID_DEPTH) != 0)
        {
            showString("DEPTH FAIL      ", 1);
            return;
        }

        int len = readDataBlock(SLAVE_ID_DEPTH);

        if(len != 2)
        {
            showString("DEPTH LEN FAIL", 1);
            return;
        }

        sprintf(tmp, "Depth: %02X %02X     ", rxBuf[0], rxBuf[1]);
        showString(tmp, 1);
    }

    if(mode == 2)
    {
        int t1;
        if(busWriteByte(BUS_CMD_TEMP, SLAVE_ID_TEMP) != 0)
        {
            showString("TEMP FAIL       ", 1);
            return;
        }

        int len = readDataBlock(SLAVE_ID_TEMP);

        if(len != 5)
        {
            showString("TEMP LEN FAIL   ", 1);
            return;
        }

        sprintf(tmp, "T:%02X %02X %02X %02X %02X", rxBuf[0], rxBuf[1], rxBuf[2], rxBuf[3], rxBuf[4]);
        showString(tmp, 0);

        if(busWriteByte(BUS_CMD_TEMP, IRQ_DISTRO) != 0)
        {
            showString("DB Temp fail", 1);
            return;
        }

        len = readDataBlock(IRQ_DISTRO);
        if(len != 1)
        {
            showString("DB Temp len fail", 1);
            return;
        }

        t1 = rxBuf[0];


        if(busWriteByte(BUS_CMD_TEMP, IRQ_BALANCER) != 0)
        {
            showString("BB Temp fail", 1);
            return;
        }
        len = readDataBlock(IRQ_BALANCER);
        if(len != 1)
        {
            showString("BB Temp len fail", 1);
            return;
        }


        sprintf(tmp, "D:%02X B: %02X      ", t1, rxBuf[0]);
        showString(tmp, 1);

    }

    if(mode == 3)
    {
        if(busWriteByte(BUS_CMD_READ_ASTATUS, SLAVE_ID_VLOW) != 0)
        {
            showString("VREAD FAIL", 0);
            return;
        }

        int len = readDataBlock(SLAVE_ID_VLOW);

        if(len != 10)
        {
            showString("VREAD BAD LEN", 0);
            return;
        }

        sprintf(tmp, "%2.3fV %2.3fA   ", (rxBuf[0] << 8 | rxBuf[1]) / 1000.0,
                                         (rxBuf[2] << 8 | rxBuf[3]) / 1000.0);
        showString(tmp, 0);
        sprintf(tmp, "%2.3fV %2.3fA   ", (rxBuf[4] << 8 | rxBuf[5]) / 1000.0,
                                         (rxBuf[6] << 8 | rxBuf[7]) / 1000.0);
        showString(tmp, 1);

    }

    if(mode == 4)
    {
        if(busWriteByte(BUS_CMD_BATTVOLTAGE, SLAVE_ID_BATTSTAT) != 0)
        {
            showString("BATTV READ FAIL ", 0);
            return;
        }

        int len = readDataBlock(SLAVE_ID_BATTSTAT);

        if(len != 14)
        {
            showString("BATTV LEN FAIL  ", 0);
            return;
        }

        //Keith: Changing these to what I think should be correct for diag. Checked balancer code.
        sprintf(tmp, "B1: %2.3fV    ", (rxBuf[0] << 8 | rxBuf[1]) / 1000.0);
        showString(tmp, 0);

        sprintf(tmp, "B2: %2.3fV    ", (rxBuf[2] << 8 | rxBuf[3]) / 1000.0);
        showString(tmp, 1);
    }

    if(mode == 5)
    {
        if(busWriteByte(BUS_CMD_BATTVOLTAGE, SLAVE_ID_BATTSTAT) != 0)
        {
            showString("BATTV READ FAIL ", 0);
            return;
        }

        int len = readDataBlock(SLAVE_ID_BATTSTAT);

        if(len != 14)
        {
            showString("BATTV LEN FAIL  ", 0);
            return;
        }

        sprintf(tmp, "B3: %2.3fV    ", (rxBuf[4] << 8 | rxBuf[5]) / 1000.0);
        showString(tmp, 0);

        sprintf(tmp, "B4: %2.3fV    ", (rxBuf[6] << 8 | rxBuf[7]) / 1000.0);
        showString(tmp, 1);
    }

    if(mode == 6)
    {
        if(busWriteByte(BUS_CMD_BATTVOLTAGE, SLAVE_ID_BATTSTAT) != 0)
        {
            showString("BATTV READ FAIL ", 0);
            return;
        }

        int len = readDataBlock(SLAVE_ID_BATTSTAT);

        if(len != 14)
        {
            showString("BATTV LEN FAIL  ", 0);
            return;
        }

        sprintf(tmp, "B5: %2.3fV    ", (rxBuf[8] << 8 | rxBuf[9]) / 1000.0);
        showString(tmp, 0);

        /* 12 and 13 are the 26V rail btw. We don't check them in diag */
        sprintf(tmp, "B6: %2.3fV    ", (rxBuf[10] << 8 | rxBuf[11]) / 1000.0);
        showString(tmp, 1);
    }

    if(mode == 7)
    {
        if(busWriteByte(BUS_CMD_BATTCURRENT, SLAVE_ID_BATTSTAT) != 0)
        {
            showString("BATTI READ FAIL ", 0);
            return;
        }

        int len = readDataBlock(SLAVE_ID_BATTSTAT);

        if(len != 12)
        {
            showString("BATTI LEN FAIL  ", 0);
            return;
        }

        sprintf(tmp, "B1: %2.3fA    ", (rxBuf[0] << 8 | rxBuf[1]) / 1000.0);
        showString(tmp, 0);

        sprintf(tmp, "B2: %2.3fA    ", (rxBuf[2] << 8 | rxBuf[3]) / 1000.0);
        showString(tmp, 1);
    }

    if(mode == 8)
    {
        if(busWriteByte(BUS_CMD_BATTCURRENT, SLAVE_ID_BATTSTAT) != 0)
        {
            showString("BATTI READ FAIL ", 0);
            return;
        }

        int len = readDataBlock(SLAVE_ID_BATTSTAT);

        if(len != 12)
        {
            showString("BATTI LEN FAIL  ", 0);
            return;
        }

        sprintf(tmp, "B3: %2.3fA    ", (rxBuf[4] << 8 | rxBuf[5]) / 1000.0);
        showString(tmp, 0);

        sprintf(tmp, "B4: %2.3fA    ", (rxBuf[6] << 8 | rxBuf[7]) / 1000.0);
        showString(tmp, 1);
    }

    if(mode == 9)
    {
        if(busWriteByte(BUS_CMD_BATTCURRENT, SLAVE_ID_BATTSTAT) != 0)
        {
            showString("BATTI READ FAIL ", 0);
            return;
        }

        int len = readDataBlock(SLAVE_ID_BATTSTAT);

        if(len != 12)
        {
            showString("BATTI LEN FAIL  ", 0);
            return;
        }

        sprintf(tmp, "B5: %2.3fA    ", (rxBuf[8] << 8 | rxBuf[9]) / 1000.0);
        showString(tmp, 0);

        sprintf(tmp, "B6: %2.3fA    ", (rxBuf[10] << 8 | rxBuf[11]) / 1000.0);
        showString(tmp, 1);
    }
}

void diagBootMode()
{
    byte mode=0;
    long j=0;

    showString("Diagnostic Mode ", 0);
    while(pollStartSw());

    while(1)
    {
        if(pollStartSw())
        {
            mode++;
            if(mode == 10)
            {
                showString("Diagnostic Mode ", 0);
                mode = 0;
            }
            showBootDiag(mode);

            j=0;
            while(pollStartSw())
            {
                j++;
                if(j == 5000)
                {
                    return;
                }
            }
        }
        showBootDiag(mode);
    }
}

/* Simple mapping, hostCmd -> busCmd */
void simpleCmd(byte cmdCode, byte replyCode, byte slaveId, byte busCmd)
{
    byte t1 = waitchar(1);
    if(t1 != cmdCode)
    {
        sendByte(HOST_REPLY_BADCHKSUM);
        return;
    }

    /* Send command to appropriate slave */
    if(busWriteByte(busCmd, slaveId) != 0)
    {
        sendByte(HOST_REPLY_FAILURE);
        return;
    }

    if(readDataBlock(slaveId) != 1)
    {
        sendByte(HOST_REPLY_FAILURE);
        return;
    }
    sendByte(replyCode);
    sendByte(rxBuf[0]);
    sendByte(replyCode+rxBuf[0]);
}


/* Check if our timeout has occurred, and if so,
   - send out speed commands to set speed to 0
   - safe all thrusters in case that didn't work
*/
void checkFailsafe()
{
#ifndef MOTORBOARD
    if(failsafeTripped == 1)
    {
        failsafeTripped = 0;

        /* Zero the speeds */
        busWriteByte(SLAVE_MM1_WRITE_CMD, SLAVE_ID_MM1);
        busWriteByte(0, SLAVE_ID_MM1);
        busWriteByte(0, SLAVE_ID_MM1);

        busWriteByte(SLAVE_MM2_WRITE_CMD, SLAVE_ID_MM2);
        busWriteByte(0, SLAVE_ID_MM2);
        busWriteByte(0, SLAVE_ID_MM2);

        busWriteByte(SLAVE_MM3_WRITE_CMD, SLAVE_ID_MM3);
        busWriteByte(0, SLAVE_ID_MM3);
        busWriteByte(0, SLAVE_ID_MM3); //Keith: Changed 1st arg from rxBuf[5] to 0 (2-17-2011)

        busWriteByte(SLAVE_MM4_WRITE_CMD, SLAVE_ID_MM4);
        busWriteByte(0, SLAVE_ID_MM4);
        busWriteByte(0, SLAVE_ID_MM4);

        busWriteByte(SLAVE_MM5_WRITE_CMD, SLAVE_ID_MM5);
        busWriteByte(0, SLAVE_ID_MM5);
        busWriteByte(0, SLAVE_ID_MM5);

        busWriteByte(SLAVE_MM6_WRITE_CMD, SLAVE_ID_MM6);
        busWriteByte(0, SLAVE_ID_MM6);
        busWriteByte(0, SLAVE_ID_MM6);

        busWriteByte(BUS_CMD_THRUSTER1_OFF, SLAVE_ID_THRUSTERS);
        busWriteByte(BUS_CMD_THRUSTER2_OFF, SLAVE_ID_THRUSTERS);
        busWriteByte(BUS_CMD_THRUSTER3_OFF, SLAVE_ID_THRUSTERS);
        busWriteByte(BUS_CMD_THRUSTER4_OFF, SLAVE_ID_THRUSTERS);
        busWriteByte(BUS_CMD_THRUSTER5_OFF, SLAVE_ID_THRUSTERS);
        busWriteByte(BUS_CMD_THRUSTER6_OFF, SLAVE_ID_THRUSTERS);
    }
#else
    if(failsafeTripped == 1)
    {
        failsafeTripped= 0;
        busWriteByte(BUS_CMD_KILL_MOTORS, SLAVE_ID_MOTOR);

        busWriteByte(BUS_CMD_THRUSTER1_OFF, SLAVE_ID_THRUSTERS);
        busWriteByte(BUS_CMD_THRUSTER2_OFF, SLAVE_ID_THRUSTERS);
        busWriteByte(BUS_CMD_THRUSTER3_OFF, SLAVE_ID_THRUSTERS);
        busWriteByte(BUS_CMD_THRUSTER4_OFF, SLAVE_ID_THRUSTERS);
        busWriteByte(BUS_CMD_THRUSTER5_OFF, SLAVE_ID_THRUSTERS);
        busWriteByte(BUS_CMD_THRUSTER6_OFF, SLAVE_ID_THRUSTERS);
    }
#endif
}

/* Flash the yellow LED on and off n times */
/* Will block until the blinking is done */
void blink(byte n)
{
    long j;
    byte i;
    for(i=0; i<n; i++)
    {
        LAT_LED_ACT = LED_ON;
        for(j=0; j<30000; j++);
        LAT_LED_ACT = ~LED_ON;
        for(j=0; j<50000; j++);
   }
   for(j=0; j<100000; j++);
}

/* If OK, returns 0. If error, returns non 0 */
byte pingChip(byte irq)
{
    switch(busWriteByte(BUS_CMD_PING, irq))
    {
        case BUS_ERROR:
        case BUS_FAILURE:
            return 1;
        break;

        case 0:
        {
            byte len = readDataBlock(irq);

            switch(len)
            {
                case 0:
                    return 0;
                break;

                case BUS_ERROR:
                case BUS_FAILURE:
                default:
                    return 1;
            }
            break;
        }
    }
    return 0;
}

/* If there is a problem, ERR light is on and ACT light blinks (1+num of failed irq) times */
/* .. because blinking 0 times is hard to see */
void post()
{
    byte i, j;
    for(i=0; i<NUM_SLAVES; i++)
    {
        if(pingChip(postList[i]) != 0) /* Someone failed */
        {
            LAT_LED_ERR = LED_ON;
            for(j=0; j<2; j++)
            {
                /* Flash error light 3 times */
                blink(postList[i]+1);
            }
        }
    }
}

int main(void)
{
    long t1, t2;
    long j=0;
    byte i;
    _TRISF0 = TRIS_IN;
    TRIS_USBDETECT = TRIS_IN;

    ADPCFG = 0xFFFF;
    LATB = 0;
    TRISB = 0x000;    /* We want ALL of portB to be outputs. */

    for(i=0; i<NUM_SLAVES; i++)
        setReq(i, 0);

    LAT_LED_ACT = LED_ON;
    LAT_LED_ERR = LED_ON;

    TRIS_LED_ACT = TRIS_OUT;
    TRIS_LED_ERR = TRIS_OUT;

    initBus();

#ifdef HAS_UART
    initInterruptUarts();
#endif

    /* Don't run POST until we've given SONAR enough time to start */
    for(j=0; j<1500000; j++);


    LAT_LED_ACT = ~LED_ON;
    LAT_LED_ERR = ~LED_ON;

    post();

    for(j=0; j<20000; j++);

    unsigned char emptyLine[]="                ";

    showString(emptyLine, 0);
    showString(emptyLine, 1);

    for(j=0; j<25000; j++);

    showString("Diagnostic?", 0);

    for(j=0; j<25000 && (pollStartSw() == 0); j++);

    if(pollStartSw())
        diagBootMode();


    LAT_LED_ACT = ~LED_ON;


    if(IN_USBDETECT != USB_PRESENT)
    {
        showString("   Self-Test    ", 0);
        showString("    Complete    ", 1);

//         showString("No USB link     ", 0);
//         showString("detected        ", 1);

    }

    LAT_LED_ACT = ~LED_ON;
    LAT_LED_ERR = LED_ON;

    while(IN_USBDETECT != USB_PRESENT)
    {
        for(j=0; j<50000; j++);
        LAT_LED_ACT = ~LAT_LED_ACT;
        LAT_LED_ERR = ~LAT_LED_ERR;
    }

    LAT_LED_ACT = ~LED_ON;
    LAT_LED_ERR = ~LED_ON;

    initMasterUart();


    showString("Starting up...  ", 0);
    showString("                ", 1);

    while(1)
    {
        actLight();
        byte c = waitchar(0);   // This returns if USB disappears

//        Try this again after optoisolation got put in...
        if(IN_USBDETECT != USB_PRESENT)
        {
            showString("Lost Mini...    ", 0);
            showString("      ;_;       ", 1);

            LAT_LED_ACT = ~LED_ON;  // Red on
            LAT_LED_ERR = LED_ON;   // Yellow off

            while(IN_USBDETECT != USB_PRESENT)
            {
                for(j=0; j<50000; j++);
                LAT_LED_ACT = ~LAT_LED_ACT;
                LAT_LED_ERR = ~LAT_LED_ERR;
            }

            showString("USB Restored... ", 0);
            showString("     d^_^b      ", 1);

            continue;
        }

        switch(c)
        {
            case HOST_CMD_SYNC:
            {
                sendByte(HOST_REPLY_SUCCESS);
                break;
            }


            case HOST_CMD_PING:
            {
                t1 = waitchar(1); /* Wait for identical checksum. This method is repeated below for 1-byte commands */
                if(t1 == HOST_CMD_PING) 
                    sendByte(HOST_REPLY_SUCCESS);
                else
                    sendByte(HOST_REPLY_BADCHKSUM);

                break;
            }


            case HOST_CMD_SYSCHECK:
            {
                byte err=0;

                t1 = waitchar(1);
                if(t1 != HOST_CMD_SYSCHECK)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                for(i=0; i<NUM_SLAVES; i++)
                {
                    switch(busWriteByte(BUS_CMD_PING, i))
                    {
                        case BUS_ERROR:
                            err++;
                        break;

                        case BUS_FAILURE:
                            err++;
                        break;

                        case 0:
                        {
                            byte len = readDataBlock(i);

                            switch(len)
                            {
                                case 0:
                                break;

                                case BUS_ERROR:
                                case BUS_FAILURE:
                                default:
                                    err++;
                            }
                        }
                        break;
                    }

                }

                if(err == 0)
                    sendByte(HOST_REPLY_SUCCESS);
                else
                    sendByte(HOST_REPLY_FAILURE);

                break;
            }


            case HOST_CMD_DEPTH:
            {
                t1 = waitchar(1);
                if(t1 != HOST_CMD_DEPTH)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_DEPTH, SLAVE_ID_DEPTH) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                int len = readDataBlock(SLAVE_ID_DEPTH);

                if(len != 2)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_DEPTH);
                sendByte(rxBuf[0]);
                sendByte(rxBuf[1]);
                sendByte(chksum(rxBuf, 2, HOST_REPLY_DEPTH));
                break;
            }

            case HOST_CMD_THRUSTERSTATE:
            {
                simpleCmd(HOST_CMD_THRUSTERSTATE, HOST_REPLY_THRUSTERSTATE, SLAVE_ID_THRUSTERS, BUS_CMD_THRUSTER_STATE);
                break;
            }

            case HOST_CMD_READ_OVR:
            {
                simpleCmd(HOST_CMD_READ_OVR, HOST_REPLY_OVR, SLAVE_ID_THRUSTERS, BUS_CMD_READ_OVR);
                break;
            }

//kanga - 7/3/2013
            case HOST_CMD_CAM_RELAY_ON:
            {
                t1= waitchar(1);
                if(t1 != HOST_CMD_CAM_RELAY_ON)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_CAM_RELAY_ON, SLAVE_ID_DEPTH) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);

                break;
                
            }

            case HOST_CMD_CAM_RELAY_OFF:
            {
                 t1= waitchar(1);
                if(t1 != HOST_CMD_CAM_RELAY_OFF)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_CAM_RELAY_OFF, SLAVE_ID_DEPTH) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);

                break;
            }


            case HOST_CMD_BATTSTATE:
            {
                simpleCmd(HOST_CMD_BATTSTATE, HOST_REPLY_BATTSTATE, SLAVE_ID_BATTSTAT, BUS_CMD_BATTSTATE);
                break;
            }


            case HOST_CMD_BARSTATE:
            {
                simpleCmd(HOST_CMD_BARSTATE, HOST_REPLY_BARSTATE, SLAVE_ID_BARS, BUS_CMD_BAR_STATE);
                break;
            }

            case HOST_CMD_READ_OVRLIMIT:
            {
                t1 = waitchar(1);
                if(t1 != HOST_CMD_READ_OVRLIMIT)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_READ_REG, SLAVE_ID_THRUSTERS) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                if(busWriteByte(0, SLAVE_ID_THRUSTERS) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                if(readDataBlock(SLAVE_ID_THRUSTERS) != 1)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                t1 = rxBuf[0]; //allow for more reading

                if(busWriteByte(BUS_CMD_READ_REG, SLAVE_ID_THRUSTERS) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                if(busWriteByte(1, SLAVE_ID_THRUSTERS) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                if(readDataBlock(SLAVE_ID_THRUSTERS) != 1)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                /* Added by Keith for consistent chksum usage 3/3/2011 */
                rxBuf[1]= t1;

                sendByte(HOST_REPLY_OVRLIMIT);
                sendByte(rxBuf[1]);
                sendByte(rxBuf[0]);
                sendByte(chksum(rxBuf, 2, HOST_REPLY_OVRLIMIT));

                break;
            }

            case HOST_CMD_SET_OVRLIMIT:
            {
                rxBuf[0] = waitchar(1);
                rxBuf[1] = waitchar(1);
                rxBuf[2] = waitchar(1);

                if(chksum(rxBuf, 2, HOST_CMD_SET_OVRLIMIT) != rxBuf[2])
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_WRITE_REG, SLAVE_ID_THRUSTERS) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                if(busWriteByte(0, SLAVE_ID_THRUSTERS) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                if(busWriteByte(rxBuf[0], SLAVE_ID_THRUSTERS) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                if(busWriteByte(BUS_CMD_WRITE_REG, SLAVE_ID_THRUSTERS) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                if(busWriteByte(1, SLAVE_ID_THRUSTERS) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                if(busWriteByte(rxBuf[1], SLAVE_ID_THRUSTERS) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            // 0=internal, 1=external
            case HOST_CMD_SWITCHPOWER:
            {
                rxBuf[0] = waitchar(1);
                rxBuf[1] = waitchar(1);

                /* Check checksum and check if command is valid */
                if((rxBuf[0] != 0 && rxBuf[0] != 1) || (chksum(rxBuf, 1, HOST_CMD_SWITCHPOWER) != rxBuf[1]))
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(rxBuf[0] == 0)
                    t1 = busWriteByte(BUS_CMD_INTPOWER, SLAVE_ID_BATTSTAT);
                else
                    t1 = busWriteByte(BUS_CMD_EXTPOWER, SLAVE_ID_BATTSTAT);

                if(t1 != 0)
                    sendByte(HOST_REPLY_FAILURE);
                else
                    sendByte(HOST_REPLY_SUCCESS);

                break;
            }


            // [0 0 0 0 0 0 S K] [W 0 B6 B5 B4 B3 B2 B1]
            case HOST_CMD_BOARDSTATUS:
            {
                t1 = waitchar(1);
                if(t1 != HOST_CMD_BOARDSTATUS) // checksum
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }
                t1 = 0;
//#if 0
                /* Read battery and water from balancer board */
                if(busWriteByte(BUS_CMD_BOARDSTATUS, SLAVE_ID_BATTSTAT) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                if(readDataBlock(SLAVE_ID_BATTSTAT) != 1)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                /* Now that all the formailities are done, we get the byte from the */
                /* Balancer Board, store it in the first byte of the message. */
                t1 = rxBuf[0]; //prepare for more reading
//#endif

                /* Read kill switch from another chip......... */
                if(busWriteByte(BUS_CMD_BOARDSTATUS, SLAVE_ID_KILLSW) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                if(readDataBlock(SLAVE_ID_KILLSW) != 1)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                /* t2 is the second reply byte
                 * If you modify what bit this is stored in, be sure to mark
                 * the change in sensorapi.h status bits for kill switch */
                t2= rxBuf[0] & 0x01; //store in bit 1

                /* This was pre the 2 byte status reply
                if(rxBuf[0] & 0x01)
                    t1 |= 0x40;
                */


                /* Read start switch from another chip......... */
                if(busWriteByte(BUS_CMD_STARTSW, SLAVE_ID_STARTSW) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                if(readDataBlock(SLAVE_ID_STARTSW) != 1)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                /* Check the byte, and or it in to bit 2 in packet t2 */
                t2 |= (rxBuf[0] & 0x01) << 1; //store in bit 2 without changing the rest

                /* This was the pre-2 byte status reply
                if(rxBuf[0] & 0x01)
                    t1 |= 0x80;
                */

                /* Prepare for use of chksum. Added by Keith 3/3/2011 */
                rxBuf[0]= t2;
                rxBuf[1]= t1;

                sendByte(HOST_REPLY_BOARDSTATUS);
                sendByte(rxBuf[0]); /* Changed from t2&t1 to rxBufs by Keith 3/3/2011 */
                sendByte(rxBuf[1]);
                sendByte(chksum(rxBuf, 2, HOST_REPLY_BOARDSTATUS));

                break;
            }



            case HOST_CMD_HARDKILL:
            {

                for(i=0; i<5; i++)
                    rxBuf[i] = waitchar(1);

                byte cflag=0;

                for(i=0; i<5; i++)
                {
                    if(rxBuf[i] != hkSafety[i])
                        cflag=1;
                }

                if(cflag == 1)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                } else {
                    if(busWriteByte(BUS_CMD_HARDKILL, SLAVE_ID_HARDKILL) != 0)
                    {
                        sendByte(HOST_REPLY_FAILURE);
                        break;
                    }
                    sendByte(HOST_REPLY_SUCCESS);
                }
                break;
            }


            case HOST_CMD_MARKER:
            {
                rxBuf[0] = waitchar(1);
                rxBuf[1] = waitchar(1);

                /* Check checksum and check if command is valid */
                if((rxBuf[0] != 0 && rxBuf[0] != 1) || (chksum(rxBuf, 1, HOST_CMD_MARKER) != rxBuf[1]))
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(rxBuf[0] == 0 ? BUS_CMD_MARKER1 : BUS_CMD_MARKER2, SLAVE_ID_MARKERS) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }


            case HOST_CMD_BACKLIGHT:
            {
                rxBuf[0] = waitchar(1);
                rxBuf[1] = waitchar(1);

                const static unsigned char blCommands[]=
                    {BUS_CMD_LCD_LIGHT_OFF, BUS_CMD_LCD_LIGHT_ON, BUS_CMD_LCD_LIGHT_FLASH}; /* Move this to top header thing? -Keith */

                /* Check checksum and check if command is valid */
                if((rxBuf[0] != 0 && rxBuf[0] != 1 && rxBuf[0] != 2) || (chksum(rxBuf, 1, HOST_CMD_BACKLIGHT) != rxBuf[1]))
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(blCommands[rxBuf[0]], SLAVE_ID_LCD) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }


            case HOST_CMD_BFIN_STATE:
            {
                rxBuf[0] = waitchar(1);
                rxBuf[1] = waitchar(1);

                const static unsigned char bfCommands[]=
                    {BUS_CMD_BFIN_STOP, BUS_CMD_BFIN_START}; /* Move to top header thingy? -Keith */

                /* Check checksum and check if command is valid */
                if((rxBuf[0] != 0 && rxBuf[0] != 1) || (chksum(rxBuf, 1, HOST_CMD_BFIN_STATE) != rxBuf[1]))
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(bfCommands[rxBuf[0]], SLAVE_ID_SONAR) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }


            case HOST_CMD_BARANIMATION:
            {
                rxBuf[0] = waitchar(1);
                rxBuf[1] = waitchar(1);

                /* Check checksum and check if command is valid */
                if((rxBuf[0] != 0 && rxBuf[0] != 1 && rxBuf[0] != 2) || (chksum(rxBuf, 1, HOST_CMD_BARANIMATION) != rxBuf[1]))
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_SET_BARMODE, SLAVE_ID_BARS) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                if(busWriteByte(rxBuf[0], SLAVE_ID_BARS) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }


                sendByte(HOST_REPLY_SUCCESS);
                break;
            }


            case HOST_CMD_SET_BARS:
            {
                rxBuf[0] = waitchar(1);
                rxBuf[1] = waitchar(1);

                if(chksum(rxBuf, 1, HOST_CMD_SET_BARS) != rxBuf[1])
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_SET_BARS, SLAVE_ID_BARS) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                if(busWriteByte(rxBuf[0], SLAVE_ID_BARS) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }


            case HOST_CMD_THRUSTERS:
            {
                for(i=0; i<5; i++)
                    rxBuf[i] = waitchar(1);

                t1 = waitchar(1);
                t2 = waitchar(1);

                byte cflag=0;
                byte cs=0;

                // Check the special sequence
                for(i=0; i<5; i++)
                {
                    cs += rxBuf[i];
                    if(rxBuf[i] != tkSafety[i])
                        cflag=1;
                }

                cs += t1 + HOST_CMD_THRUSTERS;


                /* If you mess with these arrays, please remember to update */
                /* the sensorapi.h in ../../drivers/sensor-r5/include */
                const static unsigned char tkCommands[]=
                {
                    BUS_CMD_THRUSTER1_OFF, BUS_CMD_THRUSTER2_OFF,
                    BUS_CMD_THRUSTER3_OFF, BUS_CMD_THRUSTER4_OFF,
                    BUS_CMD_THRUSTER5_OFF, BUS_CMD_THRUSTER6_OFF,

                    BUS_CMD_THRUSTER1_ON, BUS_CMD_THRUSTER2_ON,
                    BUS_CMD_THRUSTER3_ON, BUS_CMD_THRUSTER4_ON,
                    BUS_CMD_THRUSTER5_ON, BUS_CMD_THRUSTER6_ON
                };

                if(cflag == 1 || t1 > 11 || (t2 != cs))
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                } else
                {
                    if(busWriteByte(tkCommands[t1], SLAVE_ID_THRUSTERS) != 0)
                    {
                        sendByte(HOST_REPLY_FAILURE);
                        break;
                    }
                }
                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            case HOST_CMD_BARS:
            {
                rxBuf[0] = waitchar(1);
                rxBuf[1] = waitchar(1);

                /* Check checksum and check if command is valid */
                if((rxBuf[0] > 15) || (chksum(rxBuf, 1, HOST_CMD_BARS) != rxBuf[1]))
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }


                /* If you mess with these arrays, please remember to update */
                /* the sensorapi.h in ../../drivers/sensor-r5/include */
                const static unsigned char barCommands[]=
                {
                    BUS_CMD_BAR1_OFF, BUS_CMD_BAR2_OFF,
                    BUS_CMD_BAR3_OFF, BUS_CMD_BAR4_OFF,
                    BUS_CMD_BAR5_OFF, BUS_CMD_BAR6_OFF,
                    BUS_CMD_BAR7_OFF, BUS_CMD_BAR8_OFF,

                    BUS_CMD_BAR1_ON, BUS_CMD_BAR2_ON,
                    BUS_CMD_BAR3_ON, BUS_CMD_BAR4_ON,
                    BUS_CMD_BAR5_ON, BUS_CMD_BAR6_ON,
                    BUS_CMD_BAR7_ON, BUS_CMD_BAR8_ON,
                };

                if(busWriteByte(barCommands[rxBuf[0]], SLAVE_ID_BARS) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            case HOST_CMD_BATTCTL:
            {
                rxBuf[0] = waitchar(1);
                rxBuf[1] = waitchar(1);

                /* Check checksum and check if command is valid */
                if((t1 > 9) || (chksum(rxBuf, 1, HOST_CMD_BATTCTL) != rxBuf[1]))
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }


                /* If you mess with these arrays, please remember to update */
                /* the sensorapi.h in ../../drivers/sensor-r5/include */
                const static unsigned char battCommands[]=
                {
                    BUS_CMD_BATT1_OFF, BUS_CMD_BATT2_OFF,
                    BUS_CMD_BATT3_OFF, BUS_CMD_BATT4_OFF,
                    BUS_CMD_BATT5_OFF, BUS_CMD_BATT6_OFF,

                    BUS_CMD_BATT1_ON, BUS_CMD_BATT2_ON,
                    BUS_CMD_BATT3_ON, BUS_CMD_BATT4_ON,
                    BUS_CMD_BATT5_ON, BUS_CMD_BATT6_ON
                };

                if(busWriteByte(battCommands[rxBuf[0]], SLAVE_ID_BATTSTAT) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }


            /* This does not include the balancer board temperature sensor */
            case HOST_CMD_TEMPERATURE:
            {
                t1 = waitchar(1);
                if(t1 != HOST_CMD_TEMPERATURE)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                int len = 0;

                if(busWriteByte(BUS_CMD_TEMP, IRQ_DISTRO) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                len = readDataBlock(IRQ_DISTRO);
                if(len != 1)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                t1 = rxBuf[0]; //allow for more reading


                if(busWriteByte(BUS_CMD_TEMP, IRQ_BALANCER) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                len = readDataBlock(IRQ_BALANCER);
                if(len != 1)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                t2 = rxBuf[0]; //allow for more reading


                if(busWriteByte(BUS_CMD_TEMP, SLAVE_ID_TEMP) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                len = readDataBlock(SLAVE_ID_TEMP);
                if(len != 5)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                /* Prepare for chksum usage--compile all bytes */
                rxBuf[5]= t1; // Distro board temperature
                rxBuf[6]= t2; // Balancer board temperature

                sendByte(HOST_REPLY_TEMPERATURE);
// sb x x x x distro balancer
                for(i=0; i<7; i++)
                {
                    sendByte(rxBuf[i]);
                } 
                sendByte(chksum(rxBuf, 7, HOST_REPLY_TEMPERATURE));
                break;
            }


            case HOST_CMD_PRINTTEXT:
            {
                rxBuf[16] = waitchar(1);
                for(i=0; i<16; i++)
                {
                    rxBuf[i] = waitchar(1);
                }
                rxBuf[17] = waitchar(1);

                /* Check checksum and check if command is valid */
                if((rxBuf[16] > 1) || (chksum(rxBuf, 17, HOST_CMD_PRINTTEXT) != rxBuf[17]))
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                int err=0;

                for(i=0; i<16 && err==0; i++)
                {
                    err+=busWriteByte(BUS_CMD_LCD_WRITE, SLAVE_ID_LCD);
                    err+=busWriteByte(rxBuf[16]*16+i, SLAVE_ID_LCD);
                    err+=busWriteByte(rxBuf[i], SLAVE_ID_LCD);
                }

                err+=busWriteByte(BUS_CMD_LCD_REFRESH, SLAVE_ID_LCD);

                if(err != 0)
                    sendByte(HOST_REPLY_FAILURE);
                else
                    sendByte(HOST_REPLY_SUCCESS);

                break;
            }

            /* May vastly change */
            case HOST_CMD_SONAR:
            {
                t1 = waitchar(1);
                if(t1 != HOST_CMD_SONAR)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_SONAR, SLAVE_ID_SONAR) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }


                int len = readDataBlock(SLAVE_ID_SONAR);
                if(len != SONAR_PACKET_LEN)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SONAR);
                for(i=0; i<SONAR_PACKET_LEN; i++)
                {
                    sendByte(rxBuf[i]);
                }
                sendByte(chksum(rxBuf, SONAR_PACKET_LEN, HOST_REPLY_SONAR));
                break;
            }

            /* May vastly change */
            case HOST_CMD_BFRESET:
            {
                t1 = waitchar(1);
                if(t1 != HOST_CMD_BFRESET)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_BFRESET, SLAVE_ID_SONAR) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }


            case HOST_CMD_RUNTIMEDIAG:
            {
                rxBuf[0] = waitchar(1);
                rxBuf[1] = waitchar(1);

                /* Check checksum and check if command is valid */
                if((rxBuf[0] != 0 && rxBuf[0] != 1) || (chksum(rxBuf, 1, HOST_CMD_RUNTIMEDIAG) != rxBuf[1]))
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                diagMsg= rxBuf[0];

                if(diagMsg == 0)
                    showString("Runtime Diag Off", 1);
                else
                    showString("Runtime Diag On ", 1);

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            case HOST_CMD_SETSPEED:
            {
                setMotorFailsafe();
                /* 12 bytes of speed, plus checksum */
                for(i=0; i<13; i++)
                {
                    rxBuf[i] = waitchar(1);
                }

                if(chksum(rxBuf, 12, HOST_CMD_SETSPEED) != rxBuf[12])
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                t1 = 0;

#ifndef MOTORBOARD
                if(busWriteByte(SLAVE_MM1_WRITE_CMD, SLAVE_ID_MM1) != 0) t1++;
                if(busWriteByte(rxBuf[0], SLAVE_ID_MM1) != 0) t1++;
                if(busWriteByte(rxBuf[1], SLAVE_ID_MM1) != 0) t1++;

                if(busWriteByte(SLAVE_MM2_WRITE_CMD, SLAVE_ID_MM2) != 0) t1++;
                if(busWriteByte(rxBuf[2], SLAVE_ID_MM2) != 0) t1++;
                if(busWriteByte(rxBuf[3], SLAVE_ID_MM2) != 0) t1++;

                if(busWriteByte(SLAVE_MM3_WRITE_CMD, SLAVE_ID_MM3) != 0) t1++;
                if(busWriteByte(rxBuf[4], SLAVE_ID_MM3) != 0) t1++;
                if(busWriteByte(rxBuf[5], SLAVE_ID_MM3) != 0) t1++;

                if(busWriteByte(SLAVE_MM4_WRITE_CMD, SLAVE_ID_MM4) != 0) t1++;
                if(busWriteByte(rxBuf[6], SLAVE_ID_MM4) != 0) t1++;
                if(busWriteByte(rxBuf[7], SLAVE_ID_MM4) != 0) t1++;

                if(busWriteByte(SLAVE_MM5_WRITE_CMD, SLAVE_ID_MM5) != 0) t1++;
                if(busWriteByte(rxBuf[8], SLAVE_ID_MM5) != 0) t1++;
                if(busWriteByte(rxBuf[9], SLAVE_ID_MM5) != 0) t1++;

                if(busWriteByte(SLAVE_MM6_WRITE_CMD, SLAVE_ID_MM6) != 0) t1++;
                if(busWriteByte(rxBuf[10], SLAVE_ID_MM6) != 0) t1++;
                if(busWriteByte(rxBuf[11], SLAVE_ID_MM6) != 0) t1++;
#else
                busWriteByte(BUS_CMD_SET_MOT_SPEEDS, SLAVE_ID_MOTOR);

                for(i= 0;(i >> 1) < 6;i+= 2) //Send 2 at a time, 6 times.
                {
                    if(busWriteByte(convertSpeed(rxBuf[i], rxBuf[i+1]), SLAVE_ID_MOTOR))
                    {
                        t1++;
                    }
                }
#endif

                int speed;
                
                /* Inform distro board of new speeds */
                if(busWriteByte(BUS_CMD_MOTRSPEEDS, SLAVE_ID_THRUSTERS) == 0)
                {
                    for(i=0; i<6; i++)
                    {
                        speed = (rxBuf[2*i] << 8) | (rxBuf[2*i+1]);
                        if(speed < 0)
                            speed = -speed; //Wha?

                        if(busWriteByte((speed >> 1), SLAVE_ID_THRUSTERS) != 0) t1++;
                    }
                } else {
                    t1++;
                }

                if(t1 == 0)
                    sendByte(HOST_REPLY_SUCCESS);
                else
                    sendByte(HOST_REPLY_FAILURE);
                break;
           }

           case HOST_CMD_MOTOR_READ:
           {
                unsigned char resp[6];

                t1 = waitchar(1);
                if(t1 != HOST_CMD_MOTOR_READ)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                t1 = 0; //error flag

                if(busWriteByte(SLAVE_MM1_READ_CMD, SLAVE_ID_MM1) != 0) t1++;
                if(readDataBlock(SLAVE_ID_MM1) != 1) t1++;
                resp[0] = rxBuf[0];

                if(busWriteByte(SLAVE_MM2_READ_CMD, SLAVE_ID_MM2) != 0) t1++;
                if(readDataBlock(SLAVE_ID_MM2) != 1) t1++;
                resp[1] = rxBuf[0];

                if(busWriteByte(SLAVE_MM3_READ_CMD, SLAVE_ID_MM3) != 0) t1++;
                if(readDataBlock(SLAVE_ID_MM3) != 1) t1++;
                resp[2] = rxBuf[0];

                if(busWriteByte(SLAVE_MM4_READ_CMD, SLAVE_ID_MM4) != 0) t1++;
                if(readDataBlock(SLAVE_ID_MM4) != 1) t1++;
                resp[3] = rxBuf[0];

                if(busWriteByte(SLAVE_MM5_READ_CMD, SLAVE_ID_MM5) != 0) t1++;
                if(readDataBlock(SLAVE_ID_MM5) != 1) t1++;
                resp[4] = rxBuf[0];

                if(busWriteByte(SLAVE_MM6_READ_CMD, SLAVE_ID_MM6) != 0) t1++;
                if(readDataBlock(SLAVE_ID_MM6) != 1) t1++;
                resp[5] = rxBuf[0];

                if(t1 != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_CMD_MOTOR_REPLY);
                for(i=0; i<6; i++)
                {
                    sendByte(resp[i]);
                }
                sendByte(chksum(resp, 6, HOST_CMD_MOTOR_REPLY)); //note this one uses resp, not rxBuf!

                break;
            }

            case HOST_CMD_IMOTOR:
            {
                rxBuf[0] = waitchar(1);
                if(rxBuf[0] != HOST_CMD_IMOTOR)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_READ_IMOTOR, SLAVE_ID_IMOTOR) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                int len = readDataBlock(SLAVE_ID_IMOTOR);

                if(len != 16)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_IMOTOR);
                for(i=0; i<16; i++)
                {
                    sendByte(rxBuf[i]);
                }
                sendByte(chksum(rxBuf, 16, HOST_REPLY_IMOTOR));

                break;
            }

            case HOST_CMD_VLOW:
            {
                t1 = waitchar(1);
                if(t1 != HOST_CMD_VLOW)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_READ_ASTATUS, SLAVE_ID_VLOW) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                int len = readDataBlock(SLAVE_ID_VLOW);

                if(len != 10)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_VLOW);
                for(i=0; i<10; i++)
                {
                    sendByte(rxBuf[i]);
                }
                sendByte(chksum(rxBuf, 10, HOST_REPLY_VLOW));

                break;
            }

            case HOST_CMD_BATTVOLTAGE:
            {
                t1 = waitchar(1);
                if(t1 != HOST_CMD_BATTVOLTAGE)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_BATTVOLTAGE, SLAVE_ID_BATTSTAT) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                int len = readDataBlock(SLAVE_ID_BATTSTAT);

                if(len != 14)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_BATTVOLTAGE);
                for(i=0; i<14; i++)
                {
                    sendByte(rxBuf[i]);
                }
                sendByte(chksum(rxBuf, 14, HOST_REPLY_BATTVOLTAGE));

                break;
            }


            case HOST_CMD_BATTCURRENT:
            {
                t1 = waitchar(1);
                if(t1 != HOST_CMD_BATTCURRENT)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_BATTCURRENT, SLAVE_ID_BATTSTAT) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                int len = readDataBlock(SLAVE_ID_BATTSTAT);

                if(len != 12)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_BATTCURRENT);
                for(i=0; i<12; i++)
                {
                    sendByte(rxBuf[i]);
                }
                sendByte(chksum(rxBuf, 12, HOST_REPLY_BATTCURRENT));

                break;
            }

            case HOST_CMD_DVL_ON:
            {
                t1= waitchar(1);
                if(t1 != HOST_CMD_DVL_ON)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_DVL_ON, SLAVE_ID_DVL) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);

                break;
            }

            case HOST_CMD_DVL_OFF:
            {
                t1= waitchar(1);
                if(t1 != HOST_CMD_DVL_OFF)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_DVL_OFF, SLAVE_ID_DVL) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);

                break;
            }

            case HOST_CMD_MAG_PWR_ON:
            {
                if(waitchar(1) != HOST_CMD_MAG_PWR_ON) {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_MAG_PWR_ON, SLAVE_ID_MARKERS) != 0) {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            case HOST_CMD_MAG_PWR_OFF:
            {
                if(waitchar(1) != HOST_CMD_MAG_PWR_OFF) {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_MAG_PWR_OFF, SLAVE_ID_MARKERS) != 0) {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            case HOST_CMD_FIRE_TORP_1:
            {
                if(waitchar(1) != HOST_CMD_FIRE_TORP_1) {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_FIRE_TORP_1, SLAVE_ID_TORPEDO) != 0) {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            case HOST_CMD_FIRE_TORP_2:
            {
                if(waitchar(1) != HOST_CMD_FIRE_TORP_2) {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_FIRE_TORP_2, SLAVE_ID_TORPEDO) != 0) {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            case HOST_CMD_ARM_TORP_1:
            {
                if(waitchar(1) != HOST_CMD_ARM_TORP_1) {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_ARM_TORP_1, SLAVE_ID_TORPEDO) != 0) {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            case HOST_CMD_ARM_TORP_2:
            {
                if(waitchar(1) != HOST_CMD_ARM_TORP_2) {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_ARM_TORP_2, SLAVE_ID_TORPEDO) != 0) {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            case HOST_CMD_VOID_TORP_1:
            {
                if(waitchar(1) != HOST_CMD_VOID_TORP_1) {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_VOID_TORP_1, SLAVE_ID_TORPEDO) != 0) {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            case HOST_CMD_VOID_TORP_2:
            {
                if(waitchar(1) != HOST_CMD_VOID_TORP_2) {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_VOID_TORP_2, SLAVE_ID_TORPEDO) != 0) {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            case HOST_CMD_EXT_GRABBER:
            {
                if(waitchar(1) != HOST_CMD_EXT_GRABBER) {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_EXT_GRABBER, SLAVE_ID_GRABBER) != 0) {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            case HOST_CMD_RET_GRABBER:
            {
                if(waitchar(1) != HOST_CMD_RET_GRABBER) {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_RET_GRABBER, SLAVE_ID_GRABBER) != 0) {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            case HOST_CMD_VOID_GRABBER:
            {
                if(waitchar(1) != HOST_CMD_VOID_GRABBER) {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_VOID_GRABBER, SLAVE_ID_GRABBER) != 0) {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            case HOST_CMD_VOID_PNEU:
            {
                if(waitchar(1) != HOST_CMD_VOID_PNEU) {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_VOID_PNEU, SLAVE_ID_GRABBER) != 0) {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            case HOST_CMD_OFF_PNEU:
            {
                if(waitchar(1) != HOST_CMD_OFF_PNEU) {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_OFF_PNEU, SLAVE_ID_GRABBER) != 0) {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            case HOST_CMD_DERPY_ON:
            {
                if(waitchar(1) != HOST_CMD_DERPY_ON) {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_DERPY_ON, SLAVE_ID_THRUSTERS) != 0) {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            case HOST_CMD_DERPY_OFF:
            {
                if(waitchar(1) != HOST_CMD_DERPY_OFF) {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_DERPY_OFF, SLAVE_ID_THRUSTERS) != 0) {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                /* This one is a bit oddball in that it sends two commands to
                   two boards... */
                if(busWriteByte(BUS_CMD_STOP_DERPY, SLAVE_ID_MOTOR) != 0) {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            case HOST_CMD_STOP_DERPY:
            {
                if(waitchar(1) != HOST_CMD_STOP_DERPY) {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_STOP_DERPY, SLAVE_ID_MOTOR) != 0) {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            case HOST_CMD_SET_DERPY:
            {
                rxBuf[0] = waitchar(1); // Speed MSB
                rxBuf[1] = waitchar(1); // Speed LSB
                rxBuf[2] = waitchar(1); // D
                rxBuf[3] = waitchar(1); // E
                rxBuf[4] = waitchar(1); // R
                rxBuf[5] = waitchar(1); // P
                rxBuf[6] = waitchar(1); // checksum

                if(chksum(rxBuf, 6, HOST_CMD_SET_DERPY) != rxBuf[6])
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(rxBuf[2] != 'D' || rxBuf[3] != 'E' || rxBuf[4] != 'R' || rxBuf[5] != 'P')
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                if(busWriteByte(BUS_CMD_SET_DERPY, SLAVE_ID_MOTOR) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                if(busWriteByte(convertSpeed(rxBuf[0], rxBuf[1]), SLAVE_ID_MOTOR) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }
        }
    }
}


#ifdef SBR5
    #ifdef SBR7
        #error You defined SBR5 and SBR7. So, which is it?
    #endif
#endif

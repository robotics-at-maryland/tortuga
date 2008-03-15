#include <p30fxxxx.h>
#include "buscodes.h"
#include <stdio.h>

#define SENSORBOARD_IC1
#include "uart.c"

_FOSC( CSW_FSCM_OFF & ECIO );
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

#define LAT_LED_ERR     _LATB6
#define TRIS_LED_ERR    _TRISB6


#define IN_USBDETECT    _RC15
#define TRIS_USBDETECT  _TRISC15


#define IRQ_IC2         2
#define IRQ_IC3         1
#define IRQ_IC4         0
#define IRQ_DISTRO      3
#define IRQ_BALANCER    4
#define IRQ_SONAR       5

#define SLAVE_ID_POWERBOARD IRQ_DISTRO


#define SLAVE_ID_DEPTH      IRQ_IC4
#define SLAVE_ID_THRUSTERS  IRQ_DISTRO
#define SLAVE_ID_MARKERS    IRQ_DISTRO
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


/* No sonar? No power board either...*/
#define NUM_SLAVES  3

static const unsigned char hkSafety[]={0xDE, 0xAD, 0xBE, 0xEF, 0x3E};
static const unsigned char tkSafety[]={0xB1, 0xD0, 0x23, 0x7A, 0x69};
static const unsigned char cdSafety[]={0xBA, 0xDB, 0xEE, 0xEF, 0x4A};


void processRuntimeDiag();

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

/* Wait for a byte on the serial console */
unsigned char waitchar(byte timeout)
{
    long waitTime=0;
    byte x;

    U1STAbits.OERR = 0;
    U1STAbits.FERR = 0;
    U1STAbits.PERR = 0;
    U1STAbits.URXDA = 0;

    while(U1STAbits.URXDA == 0)
    {
        if(diagMsg && waitTime++ == DIAG_TIMEOUT)
        {
            processRuntimeDiag();
            waitTime=0;
        }
    }

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
        _LATB0 = val;

    if(req == 1)
        _LATB1 = val;

    if(req == 2)
        _LATB2 = val;

    if(req == 3)
        _LATB4 = val;

    if(req == 4)
        _LATB3 = val;

    if(req == 5)
        _LATB6 = val;
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


void initMasterUart()
{
    U1MODE = 0x0000;
//    U1BRG = 15;  /* 7 for 230400, 15 for 115200 194 for 9600  AT 30 MIPS*/
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

int pollStatus()
{
#warning WRITE NEW STATUS FUNCTION
    return 0;
}


byte pollThrusterState()
{
#warning WRITE NEW THRUSTER STATE FUNCTION
    return -1;
}


/* Run a bit of the run-time diagnostic message system */
void processRuntimeDiag()
{
#warning WRITE NEW RUNTIME DIAGNOSTIC FUNCTION
}


void showBootDiag(int mode)
{
    unsigned char tmp[16];
    if(mode == 0)
    {
        //sprintf(tmp, "Status: %02X      ", pollStatus());

#warning THESE BITS MAY CHANGE BASED ON SCHEMATIC

        byte sta = pollStatus();
        sprintf(tmp, "Sta: %02X %c%c%c%c%c%c%c%c", sta,
            (sta & 0x80) ? 'S' : '-',
            (sta & 0x40) ? '?' : '-',
            (sta & 0x20) ? '1' : '-',
            (sta & 0x10) ? '2' : '-',
            (sta & 0x08) ? '3' : '-',
            (sta & 0x04) ? '4' : '-',
            (sta & 0x02) ? 'K' : '-',
            (sta & 0x01) ? 'W' : '-');

        showString(tmp, 1);
    }

    if(mode == 1)
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
        showString(tmp, 1);
    }
}

void showIdent()
{
    byte i=0;
    long j=0;
    unsigned char tmp[16];

    while(!(pollStatus() & 0x80));

    for(i=0; i<NUM_SLAVES; i++)
    {
        sprintf(tmp, "Ident IRQ %d:    ", i);
        showString(tmp, 0);

        /* Don't mix the strings */
        for(j=0; j<17; j++)
            rxBuf[j]=0;

        if(busWriteByte(BUS_CMD_ID, i) != 0)
        {
            showString("<Write Fail>    ", 1);
        } else
        {
            byte len = readDataBlock(i);

            if(len > 0)
            {
                for(j=len; j<16; j++)
                    rxBuf[j]=32;

                showString(rxBuf, 1);
            } else
            {
                showString("<Read Fail>     ", 1);
            }
        }

        while(pollStatus() & 0x80);
        while(!(pollStatus() & 0x80));
    }
    showString("Diagnostic Mode ", 0);
}

void diagBootMode()
{
    byte mode=0;
    long j=0;

    showString("Diagnostic Mode ", 0);
    while(pollStatus() & 0x80);

    while(1)
    {
        if(pollStatus() & 0x80)
        {
            mode++;
            if(mode == 3)
            {
                showIdent();
                mode = 0;
            }
            showBootDiag(mode);

            j=0;
            while(pollStatus() & 0x80)
            {
                j++;
                if(j == 25000)
                {
                    return;
                }
            }
        }
        showBootDiag(mode);
    }
}


int main(void)
{
    long j=0;
    byte i;
    _TRISF0 = TRIS_IN;
    TRIS_USBDETECT = TRIS_IN;

    initBus();

    for(i=0; i<NUM_SLAVES; i++)
        setReq(i, 0);

    ADPCFG = 0xFFFF;
    LATB = 0;
    TRISB = 0;

#ifdef HAS_UART
    initInterruptUarts();
#endif

    LAT_LED_ACT = LED_ON;
    LAT_LED_ERR = LED_ON;


    TRIS_LED_ACT = TRIS_OUT;
    TRIS_LED_ERR = TRIS_OUT;


    for(j=0; j<25000; j++);

    LAT_LED_ERR = ~LED_ON;


    unsigned char emptyLine[]="                ";

    showString(emptyLine, 0);
    showString(emptyLine, 1);

    for(j=0; j<25000; j++);

    TRIS_USBDETECT = TRIS_IN;

    showString("Diagnostic?", 0);

    for(j=0; j<250000 && ((pollStatus() & 0x80) == 0); j++);

    if(pollStatus() & 0x80)
        diagBootMode();


    LAT_LED_ACT = ~LED_ON;


    if(IN_USBDETECT == 0)
    {
        showString("  Moan for me,  ", 0);
        showString("    BITCH !     ", 1);
    }

    LAT_LED_ACT = ~LED_ON;
    LAT_LED_ERR = LED_ON;

    while(IN_USBDETECT == 0)
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
        LAT_LED_ACT = ~LED_ON;
        byte c = waitchar(0);
        LAT_LED_ACT = LED_ON;

        long t1, t2;

        switch(c)
        {
            case HOST_CMD_SYNC:
            {
                sendByte(HOST_REPLY_SUCCESS);
                break;
            }


            case HOST_CMD_PING:
            {
                t1 = waitchar(1);
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
                byte cs = HOST_REPLY_DEPTH+rxBuf[0]+rxBuf[1];
                sendByte(cs);
                break;
            }

            case HOST_CMD_THRUSTERSTATE:
            {
                t1 = waitchar(1);
                if(t1 != HOST_CMD_THRUSTERSTATE)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                /* Read thruster state from distro board */
                if(busWriteByte(BUS_CMD_THRUSTER_STATE, SLAVE_ID_THRUSTERS) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                if(readDataBlock(SLAVE_ID_THRUSTERS) != 1)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }
                sendByte(HOST_REPLY_THRUSTERSTATE);
                sendByte(rxBuf[0]);
                sendByte(HOST_REPLY_THRUSTERSTATE+rxBuf[0]);

                break;
            }


            case HOST_CMD_BARSTATE:
            {
                t1 = waitchar(1);
                if(t1 != HOST_CMD_BARSTATE)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                /* Read thruster state from distro board */
                if(busWriteByte(BUS_CMD_BAR_STATE, SLAVE_ID_BARS) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                if(readDataBlock(SLAVE_ID_BARS) != 1)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }
                sendByte(HOST_REPLY_BARSTATE);
                sendByte(rxBuf[0]);
                sendByte(HOST_REPLY_BARSTATE+rxBuf[0]);

                break;
            }

            // [S  K  W B1 B2 B3 B4 B5]
            case HOST_CMD_BOARDSTATUS:
            {
                t1 = waitchar(1);
                if(t1 != HOST_CMD_BOARDSTATUS)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }
                t1 = 0;
#if 0
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

                t1 = rxBuf[0];



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

                if(rxBuf[0] & 0x01)
                    t1 |= 0x40;
#endif

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

                if(rxBuf[0] & 0x01)
                    t1 |= 0x80;

                sendByte(HOST_REPLY_BOARDSTATUS);
                sendByte(t1);
                sendByte(HOST_REPLY_BOARDSTATUS+t1);

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
                } else
                {
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
                t1 = waitchar(1);
                t2 = waitchar(1);

                if((t1 != 0 && t1 != 1) || (t1+HOST_CMD_MARKER != t2))
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(t1==0 ? BUS_CMD_MARKER1 : BUS_CMD_MARKER2, SLAVE_ID_MARKERS) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }


            case HOST_CMD_BACKLIGHT:
            {
                t1 = waitchar(1);
                t2 = waitchar(1);

                const static unsigned char blCommands[]=
                        {BUS_CMD_LCD_LIGHT_OFF, BUS_CMD_LCD_LIGHT_ON, BUS_CMD_LCD_LIGHT_FLASH};

                if((t1 != 0 && t1 != 1 && t1 != 2) || (t1+HOST_CMD_BACKLIGHT != t2))
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(blCommands[t1], SLAVE_ID_LCD) != 0)
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
                t1 = waitchar(1);
                t2 = waitchar(1);

                if(t1 + HOST_CMD_BARS != t2 || t1 > 15)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }


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

                if(busWriteByte(barCommands[t1], SLAVE_ID_BARS) != 0)
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
#if 0
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

                t1 = rxBuf[0];
#endif
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

                sendByte(HOST_REPLY_TEMPERATURE);

                byte cs=0;

                for(i=0; i<5; i++)
                {
                    cs += rxBuf[i];
                    sendByte(rxBuf[i]);
                }

                sendByte(t1);   // Distro board temperature

                sendByte(cs + t1 + HOST_REPLY_TEMPERATURE);
                break;
            }


            case HOST_CMD_PRINTTEXT:
            {
                t1 = waitchar(1);
                byte cs=HOST_CMD_PRINTTEXT+t1;

                for(i=0; i<16; i++)
                {
                    rxBuf[i] = waitchar(1);
                    cs += rxBuf[i];
                }
                t2 = waitchar(1);

                if(t2 != cs || t1 > 1)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                int err=0;

                for(i=0; i<16 && err==0; i++)
                {
                    err+=busWriteByte(BUS_CMD_LCD_WRITE, SLAVE_ID_LCD);
                    err+=busWriteByte(t1*16+i, SLAVE_ID_LCD);
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
		        byte cs=HOST_CMD_SONAR+t1;

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
                if(len != 5)
		        {
			        sendByte(HOST_REPLY_FAILURE);
			        break;
		        }

		        sendByte(HOST_REPLY_SONAR);

		        cs=0;
                for(i=0; i<5; i++)
                {
                    cs += rxBuf[i];
	                sendByte(rxBuf[i]);
		        }

		        sendByte(cs + HOST_REPLY_SONAR);
		        break;
            }

            case HOST_CMD_RUNTIMEDIAG:
            {
                t1 = waitchar(1);
                t2 = waitchar(1);

                if((t1 != 0 && t1 != 1) || (t1+HOST_CMD_RUNTIMEDIAG != t2))
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                diagMsg=t1;

                if(t1==0)
                    showString("Runtime Diag Off", 1);
                else
                    showString("Runtime Diag On ", 1);

                sendByte(HOST_REPLY_SUCCESS);
                break;
            }

            case HOST_CMD_SETSPEED:
            {
                t1 = 0; /* Error counter */

                /* 12 bytes of speed, plus checksum */
                for(i=0; i<13; i++)
                    rxBuf[i] = waitchar(1);

                for(i=0; i<12; i++)
                    t1 += rxBuf[i];

                t1 += HOST_CMD_SETSPEED;

                if(rxBuf[12] != (t1 & 0xFF))
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                t1 = 0;
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

                t1 = 0;

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
                    sendByte(resp[i]);

                sendByte(HOST_CMD_MOTOR_REPLY + resp[0] + resp[1] + resp[2] + resp[3] + resp[4] + resp[5]);

                break;
            }

            case HOST_CMD_IMOTOR:
            {
                t1 = waitchar(1);
                if(t1 != HOST_CMD_IMOTOR)
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

                byte cs=0;

                for(i=0; i<16; i++)
                {
                    cs += rxBuf[i];
                    sendByte(rxBuf[i]);
                }

                sendByte(cs + t1 + HOST_REPLY_IMOTOR);
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

                if(len != 12)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                sendByte(HOST_REPLY_VLOW);

                byte cs=0;

                for(i=0; i<12; i++)
                {
                    cs += rxBuf[i];
                    sendByte(rxBuf[i]);
                }

                sendByte(cs + HOST_REPLY_VLOW);
                break;
            }

        }
    }
}

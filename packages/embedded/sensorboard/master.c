#include <p30fxxxx.h>
#include <stdio.h>

_FOSC( CSW_FSCM_OFF & EC_PLL4 );
_FWDT ( WDT_OFF );


#define TRIS_OUT 0
#define TRIS_IN  1
#define byte unsigned char

/*
 * Bus = D1 D0 E5-E0
 * Akn = D3
 * RW  = E8
 */

/* Bus pin assignments */
#define IN_AKN      _RD3
#define LAT_AKN     _LATD3
#define TRIS_AKN    _TRISD3

#define IN_RW       _RE8
#define TRIS_RW     _TRISE8
#define LAT_RW      _LATE8


#define RW_READ     0
#define RW_WRITE    1


#define SLAVE_ID_POWERBOARD 1
#define SLAVE_ID_DEPTH      0
#define SLAVE_ID_THRUSTERS  0
#define SLAVE_ID_MARKERS    0
#define SLAVE_ID_TEMP       1
#define SLAVE_ID_LCD        2
#define SLAVE_ID_HARDKILL   1
#define SLAVE_ID_SONAR	    3

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
#define BUS_TIMEOUT     100000
#define BUS_ERROR       -1
#define BUS_FAILURE     -2

/*
 * Commands Master can send to Slaves. Command numbers among different Slaves can overlap
 * but common commands like Ping/Identify should be the same everywhere.
 * Kind of obvious what order I did these in, huh?
 */
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
//#define BUS_CMD_CHECKWATER      14
#define BUS_CMD_TEMP            15
#define BUS_CMD_BOARDSTATUS     16
#define BUS_CMD_HARDKILL        17
#define BUS_CMD_LCD_LIGHT_FLASH 18

#define BUS_CMD_THRUSTER1_OFF   19
#define BUS_CMD_THRUSTER2_OFF   20
#define BUS_CMD_THRUSTER3_OFF   21
#define BUS_CMD_THRUSTER4_OFF   22

#define BUS_CMD_THRUSTER1_ON    23
#define BUS_CMD_THRUSTER2_ON    24
#define BUS_CMD_THRUSTER3_ON    25
#define BUS_CMD_THRUSTER4_ON    26

#define BUS_CMD_SONAR	    	27

#define NUM_SLAVES  4

static const unsigned char hkSafety[]={0xDE, 0xAD, 0xBE, 0xEF, 0x3E};
static const unsigned char tkSafety[]={0xB1, 0xD0, 0x23, 0x7A, 0x69};
static const unsigned char cdSafety[]={0xBA, 0xDB, 0xEE, 0xEF, 0x4A};

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


/* Wait for a byte on the serial console */
unsigned char waitchar(byte timeout)
{
    byte x;
    U1STAbits.OERR = 0;
    U1STAbits.FERR = 0;
    U1STAbits.PERR = 0;
    U1STAbits.URXDA = 0;

    while(U1STAbits.URXDA == 0);
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
        _LATB3 = val;
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
            return BUS_FAILURE;     /* We're totally screwed */
    }

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
            return BUS_FAILURE;     /* We're totally screwed */
    }

    return 0;
}


void initUart()
{
    U1MODE = 0x0000;
//    U1BRG = 15;  /* 7 for 230400, 15 for 115200 194 for 9600  AT 30 MIPS*/
    U1BRG = 7;  /* 7 for 115200 at 15 MIPS */
    U1MODEbits.ALTIO = 1;   // Use alternate IO
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;   // Enable transmit
}


/* Send a byte to the serial console */
void sendByte(byte i)
{
    long j;

    while(U1STAbits.UTXBF);
    while(!U1STAbits.TRMT);
    U1TXREG = i;
    while(U1STAbits.UTXBF);
    while(!U1STAbits.TRMT);

   // for(j=0; j<10000; j++); /* This line can be removed, but my uart was being weird. */
}


/* Send a string to the serial console */
void sendString(unsigned char str[])
{
    byte i=0;
    for(i=0; str[i]!=0; i++)
        sendByte(str[i]);
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

byte pollStatus()
{
    if(busWriteByte(BUS_CMD_BOARDSTATUS, SLAVE_ID_POWERBOARD) != 0)
    {
        showString("STA FAIL   ", 1);
        return 0;
    }

    byte len = readDataBlock(SLAVE_ID_POWERBOARD);

    if(len!=1)
    {
        showString("STA FAIL   ", 1);
        return 0;
    }
    return rxBuf[0];
}

void showDiag(int mode)
{
    unsigned char tmp[16];
    if(mode == 0)
    {
        //sprintf(tmp, "Status: %02X      ", pollStatus());

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

void diagMode()
{
    byte mode=0;
    unsigned char tmp[16];
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
            showDiag(mode);

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
        showDiag(mode);
    }
}


int main(void)
{
    long j=0;
    long t=0, b=0;
    byte i;

    byte tmp[60];
    byte rxPtr = 0;
    byte rxLen = 0;

    initBus();

    for(i=0; i<NUM_SLAVES; i++)
        setReq(i, 0);
    initUart();

    ADPCFG = 0xFFFF;
    LATB = 0;
    TRISB = 0;


    initUart();

    sendString("\n\rMaster starting...\n\r");
    for(j=0; j<100000; j++);


    #define HOST_CMD_SYNC           0xFF

    #define HOST_CMD_PING           0x00
    #define HOST_REPLY_SUCCESS      0xBC
    #define HOST_REPLY_FAILURE      0xDF
    #define HOST_REPLY_BADCHKSUM    0xCC

    #define HOST_CMD_SYSCHECK       0x01

    #define HOST_CMD_DEPTH          0x02
    #define HOST_REPLY_DEPTH        0x03

    #define HOST_CMD_BOARDSTATUS    0x04
    #define HOST_REPLY_BOARDSTATUS  0x05

    #define HOST_CMD_HARDKILL       0x06
    #define HOST_CMD_MARKER         0x07

    #define HOST_CMD_BACKLIGHT      0x08

    #define HOST_CMD_THRUSTERS      0x09

    #define HOST_CMD_TEMPERATURE    0x0A
    #define HOST_REPLY_TEMPERATURE  0x0B

    #define HOST_CMD_PRINTTEXT      0x0C

    #define HOST_CMD_SONAR          0x0D
    #define HOST_REPLY_SONAR        0x0E


    unsigned char emptyLine[]="                ";

    showString(emptyLine, 0);
    showString(emptyLine, 1);

    for(j=0; j<100000; j++);

    showString("Diagnostic?", 0);

    for(j=0; j<100000 && ((pollStatus() & 0x80) == 0); j++);

    if(pollStatus() & 0x80)
        diagMode();


    showString("Starting up...  ", 0);
    showString("                ", 1);

    while(1)
    {
        byte c = waitchar(0);

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


            case HOST_CMD_BOARDSTATUS:
            {
                t1 = waitchar(1);
                if(t1 != HOST_CMD_BOARDSTATUS)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_BOARDSTATUS, SLAVE_ID_POWERBOARD) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                byte len = readDataBlock(SLAVE_ID_POWERBOARD);

                if(len!=1)
                {
                    sendByte(HOST_REPLY_FAILURE);
                } else
                {
                    sendByte(HOST_REPLY_BOARDSTATUS);
                    sendByte(rxBuf[0]);
                    sendByte(HOST_REPLY_BOARDSTATUS+rxBuf[0]);
                }

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

                /* Check the special sequence */
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
                    BUS_CMD_THRUSTER1_ON, BUS_CMD_THRUSTER2_ON,
                    BUS_CMD_THRUSTER3_ON, BUS_CMD_THRUSTER4_ON
                };

                if(cflag == 1 || t1 > 7 || (t2 != cs))
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


            case HOST_CMD_TEMPERATURE:
            {
                t1 = waitchar(1);
                if(t1 != HOST_CMD_TEMPERATURE)
                {
                    sendByte(HOST_REPLY_BADCHKSUM);
                    break;
                }

                if(busWriteByte(BUS_CMD_TEMP, SLAVE_ID_TEMP) != 0)
                {
                    sendByte(HOST_REPLY_FAILURE);
                    break;
                }

                int len = readDataBlock(SLAVE_ID_TEMP);

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

                sendByte(cs + HOST_REPLY_TEMPERATURE);
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
        }
    }
}

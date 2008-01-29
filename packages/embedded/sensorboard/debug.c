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


#define IN_KS       _RF0
#define TRIS_KS     _TRISF0



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

#define SLAVE_ID_MM1        0
#define SLAVE_ID_MM2        2
#define SLAVE_ID_MM3        2

#define SLAVE_MM1_WRITE_CMD BUS_CMD_SETSPEED_U1
#define SLAVE_MM2_WRITE_CMD BUS_CMD_SETSPEED_U2
#define SLAVE_MM3_WRITE_CMD BUS_CMD_SETSPEED_U1

#define SLAVE_MM1_READ_CMD  BUS_CMD_GETREPLY_U1
#define SLAVE_MM2_READ_CMD  BUS_CMD_GETREPLY_U2
#define SLAVE_MM3_READ_CMD  BUS_CMD_GETREPLY_U1

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


/* No sonar? */
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
unsigned char waitchar()
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
/*void sendByte(byte i)
{
    while(U1STAbits.UTXBF);
    while(!U1STAbits.TRMT);
    U1TXREG = i;
    while(U1STAbits.UTXBF);
    while(!U1STAbits.TRMT);
}

*/
/* Send a byte to the serial console */
void sendByte(byte i)
{
    long j;
    while(U1STAbits.UTXBF);
    U1TXREG = i;
    while(U1STAbits.UTXBF);
    for(j=0; j<2000; j++); /* This line can be removed, but my uart was being weird. */
}


/* Send a string to the serial console */
void U2SendString(unsigned char str[])
{
    byte i=0;
    for(i=0; str[i]!=0; i++)
        U2WriteByte(str[i]);
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


byte toHex(byte c)
{
    if(c >= '0' && c <= '9')
        return c - '0';

    if(c >= 'a' && c <= 'f')
        return 10+(c-'a');
}

void rawCmdLoop()
{
    long j=0;
    byte i, t;
    byte curID = 0;
    byte tmp[60];

    sendString("\n\r\n\r\n\rRaw Mode");

    while(1)
    {
        sprintf(tmp, "\n\r\n\rI%d>: ", curID);
        sendString(tmp);

        byte c = waitchar();
        sendByte(c);    /* Local Echo */

        switch(c)
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            {
                t = waitchar();
                sendByte(t);

                if(t == ' ' || t == '\n' || t == '\r' || t == 27)
                    break;

                if(busWriteByte((toHex(c) << 4) | toHex(t), curID) != 0)
                {
                    sendString("\n\rTransmit error.");
                    break;
                }

                byte len = readDataBlock(curID);

                if(len < 0)
                {
                    sendString("\n\rRecieve error.");
                    break;
                }


                sprintf(tmp, "\n\rRead %d bytes:\n\r<", len);
                sendString(tmp);

                for(i=0; i<len; i++)
                {
                    sprintf(tmp, "%02X ", rxBuf[i]);
                    sendString(tmp);
                }

                sendString(">\n\r<");

                for(i=0; i<len; i++)
                {
                    if(rxBuf[i] > 20)
                    {
                        sprintf(tmp, "%c  ", rxBuf[i]);
                        sendString(tmp);
                    } else
                        sendString("?  ");
                }
                sendString(">\n\r");
                break;
            }



            case 'i':
            {
                sendString("\n\rIRQ?\n\r>");
                t = waitchar();
                sendByte(t);
                if(t != 27 && t != '\n' && t != '\r' && t != ' ')
                    curID = t - 48;
                break;
            }

            case 'q':
            {
                sendString("\n\r\n\r\n\rStandard Mode");
                return;
                break;
            }
        }
    }
}


int main(void)
{
    long j=0;
    byte i;
    byte tmp[60];

    TRIS_KS = TRIS_IN;

    initBus();

    for(i=0; i<NUM_SLAVES; i++)
        setReq(i, 0);

    ADPCFG = 0xFFFF;
    LATB = 0;
    TRISB = 0;


    initMasterUart();
    initInterruptUarts();

    for(j=0; j<100000; j++);
//     sendString("\n\r\n\rEvil wombat's little diagnostic program.");

/*

    L - set LCD text
    B - backlight on
    b - backlight off
    R - read config register
    W - write config register
    p - ping slaves
    i - identify slaves
    d - depth query
    t - temperature query
    s - read status register

*/




    while(1)
    {
        sendString("\n\r\n\r>: ");
        byte c = waitchar();
        sendByte(c);    /* Local Echo */

        switch(c)
        {
            case 'r':
            {
                rawCmdLoop();
                break;
            }







            case 's':
            {
                  busWriteByte(BUS_CMD_BOARDSTATUS, SLAVE_ID_POWERBOARD);

                byte len = readDataBlock(SLAVE_ID_POWERBOARD);

                if(len!=1)
                {
                    sendString("Comm error.");
                } else
                {
                    sprintf(tmp, "\n\rStatus register: <%02X> [%c%c%c%c%c%c%c%c]", rxBuf[0],
                        (rxBuf[0] & 0x80) ? 'S' : '-',
                        (rxBuf[0] & 0x40) ? '?' : '-',
                        (rxBuf[0] & 0x20) ? '1' : '-',
                        (rxBuf[0] & 0x10) ? '2' : '-',
                        (rxBuf[0] & 0x08) ? '3' : '-',
                        (rxBuf[0] & 0x04) ? '4' : '-',
                        (rxBuf[0] & 0x02) ? 'K' : '-',
                        (rxBuf[0] & 0x01) ? 'W' : '-');
                    sendString(tmp);

                }


                if(busWriteByte(BUS_CMD_THRUSTER_STATE, SLAVE_ID_THRUSTERS) != 0)
                {
                    sendString("TSTA FAIL TX");
                    break;
                }

                len = readDataBlock(SLAVE_ID_THRUSTERS);

                if(len!=1)
                {
                    sendString("TSTA FAIL RX");
                    return 0;
                }

                if(IN_KS == 1)
                    rxBuf[0] |= 0x10;


                sprintf(tmp, "\n\rThrusters: [%c%c%c%c%c] ",
                    (rxBuf[0] & 0x10) ? 'K' : '-',
                    (rxBuf[0] & 0x08) ? '1' : '-',
                    (rxBuf[0] & 0x04) ? '2' : '-',
                    (rxBuf[0] & 0x02) ? '3' : '-',
                    (rxBuf[0] & 0x01) ? '4' : '-');

                sendString(tmp);


                break;
            }

            case 'L':
            {
                sendString("\n\r\n\rEnter up to 32 characters:\n\r>");
                byte data[] = "                                ";

                for(i=0; i<32; i++)
                {
                    data[i] = waitchar();
                    sendByte(data[i]);
                    if(data[i] =='\r')
                    {
                        data[i] = ' ';
                        break;
                    }
                }
                sendString("\n\r\n\rWriting data to LCD");

                for(i=0; i<32; i++)
                {
                    busWriteByte(BUS_CMD_LCD_WRITE, SLAVE_ID_LCD);
                    busWriteByte(i, SLAVE_ID_LCD);
                    busWriteByte(data[i], SLAVE_ID_LCD);
                }

                busWriteByte(BUS_CMD_LCD_REFRESH, SLAVE_ID_LCD);

                sendString("\n\rDone");

                break;
            }

            case 'B':
            {
                sendString("\n\rLCD Backlight on");
                busWriteByte(BUS_CMD_LCD_LIGHT_ON, SLAVE_ID_LCD);
                break;
            }


            case 'b':
            {
                sendString("\n\rLCD Backlight off");
                busWriteByte(BUS_CMD_LCD_LIGHT_OFF, SLAVE_ID_LCD);
                break;
            }

            case 'p':
            {
                sendString("\n\rPinging all slaves");
                for(i=0; i<NUM_SLAVES; i++)
                {
                    sprintf(tmp, "\n\rSlave #%d: ", i);
                    sendString(tmp);


                    switch(busWriteByte(BUS_CMD_PING, i))
                    {
                        case BUS_ERROR:
                            sendString("Comm Error: TX Timeout");
                        break;

                        case BUS_FAILURE:
                            sendString("Catastrophic bus failure: AKN held high");
                        break;

                        case 0:
                        {
                            byte len = readDataBlock(i);

                            switch(len)
                            {
                                case 0:
                                    sendString("Reply OK");
                                break;

                                case BUS_ERROR:
                                    sendString("Comm Error: RX Timeout");
                                break;

                                case BUS_FAILURE:
                                    sendString("Comm Error: Bus Failure during reply");
                                break;

                                default:
                                    sendString("Unknown Reply");
                            }
                        }
                        break;
                    }
                }

                break;
            }

            case 'i':
            {
                sendString("\n\rIdentifying slaves");
                for(i=0; i<NUM_SLAVES; i++)
                {
                    busWriteByte(BUS_CMD_ID, i);
                    byte len = readDataBlock(i);
                    sprintf(tmp, "\n\rSlave #%d replies (%d bytes): <", i, len);
                    sendString(tmp);
                    sendString(rxBuf);
                    sendString(">");
                }
                break;
            }

            case 'R':
            {
                sendString("\n\rId=");
                byte id = waitchar() & 0x0F;
                sendByte(id+48);

                sendString("\n\rReg=");
                byte addr = waitchar() & 0x0F;
                sendByte(addr+48);


                sendString("\n\rReading config...");
                busWriteByte(BUS_CMD_READ_REG, id);
                busWriteByte(addr, id);

                readDataBlock(id);

                sprintf(tmp, "\n\rSlave #%d config register %d is <%c", id, addr, rxBuf[0]);
                sendString(tmp);
                sendByte('>');
                break;
            }

            case 'W':
            {
                sendString("\n\rId=");
                byte id = waitchar() & 0x0F;
                sendByte(id+48);

                sendString("\n\rReg=");
                byte addr = waitchar() & 0x0F;
                sendByte(addr+48);

                sendString("\n\rVal=");
                byte val = waitchar();
                sendByte(val);

                sendString("\n\rWriting config...");
                busWriteByte(BUS_CMD_WRITE_REG, id);
                busWriteByte(addr, id);
                busWriteByte(val, id);

                sprintf(tmp, "\n\rSlave #%d config register %d set to <%c>", id, addr, val);
                sendString(tmp);
                break;
            }

            case 'd':
            {
                sendString("\n\rAverage of last 100 depth measurements on Slave: ");
                busWriteByte(BUS_CMD_DEPTH, SLAVE_ID_DEPTH);
                readDataBlock(SLAVE_ID_DEPTH);

                int depth = (rxBuf[0]<<8) | rxBuf[1];
                sprintf(tmp, "%u", depth);
                sendString(tmp);
                sendString("\n\rDone.");
                break;
            }

            case 't':
            {
                sendString("\n\rTemperature on Slave 1: ");
                busWriteByte(BUS_CMD_TEMP, SLAVE_ID_TEMP);
                int len = readDataBlock(SLAVE_ID_TEMP);

                sprintf(tmp, "\n\rData Received (%d bytes): ", len);
                sendString(tmp);

                for(i=0; i<len; i++)
                {
                    sprintf(tmp, "%u ", rxBuf[i]);
                    sendString(tmp);
                }

                sendString("\n\rDone.");
                break;
            }

            default:
            {
                sendString("\n\rUnknown command: ");
                sendByte(c);
            }


        }
    }
}

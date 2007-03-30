#include <p30fxxxx.h>

_FOSC( CSW_FSCM_OFF & XT_PLL4 );
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


#define BUS_TIMEOUT     100000
#define BYTE_READ_ERROR       256

#define BUS_CMD_PING        0
#define BUS_CMD_ID          1
#define BUS_CMD_READ_REG    2
#define BUS_CMD_WRITE_REG   3

#define NUM_SLAVES  3

byte readBus()
{
    return (PORTE & 0x3F) | (_RD0 << 6) | (_RD1 << 7);
}

void writeBus(byte b)
{
    TRISE = TRISE & 0xFFC0;
    _TRISD1 = TRIS_OUT;
    _TRISD0 = TRIS_OUT;

     LATE = (LATE & 0xFFC0) | (b & 0x3F);
    _LATD0 = (b & 0x40) >> 6;
    _LATD1 = (b & 0x80) >> 7;
}

void freeBus()
{
    _TRISD1 = TRIS_IN;
    _TRISD0 = TRIS_IN;
    TRISE = TRISE | 0x3F;
}

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
    return x & 0x7F;
}

void initBus()
{
    /* Put everything in high-impedance state */
    freeBus();
    TRIS_RW = TRIS_OUT;
    TRIS_AKN = TRIS_IN;
}

void setReq(byte req, byte val)
{
    if(req == 0)
        _LATB0 = val;

    if(req == 1)
        _LATB1 = val;

    if(req == 2)
        _LATB2 = val;
}

/* Read a byte from a given Slave */
/* Returns BUS_ERROR on error */
unsigned int busReadByte(byte req)
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
            return BYTE_READ_ERROR;
        }
    }

    /* Read the data */
    data = readBus();

    /* Drop Req */
    setReq(req, 0);

    /* Wait for Slave to release bus */
    /* Need timeout to detect Slave fault */
    while(IN_AKN == 1);

    return data;
}


/* Write a byte to a given slave */
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
            return BYTE_READ_ERROR;
        }
    }

    /* Release bus */
    freeBus();

    /* Drop Req */
    setReq(req, 0);

    /* Wait for Slave to release AKN */
    /* Need timeout to detect Slave fault */
    while(IN_AKN == 1);
    return 0;
}


void initUart()
{
    U1MODE = 0x0000;
    U1BRG = 311;
    U1MODEbits.ALTIO = 1;   // Use alternate IO
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;   // Enable transmit
}

/* Send a byte to the serial console */
void sendByte(byte i)
{
    long j;
    while(U1STAbits.UTXBF);
    U1TXREG = i;
    while(U1STAbits.UTXBF);
    for(j=0; j<10000; j++); /* This line can be removed, but my uart was being weird. */
}

/* Send a string to the serial console */
void sendString(unsigned char str[])
{
    byte i=0;
    for(i=0; str[i]!=0; i++)
        sendByte(str[i]);
}

/* General purpose bus receive buffer */
byte rxBuf[30];

int readDataBlock(byte req)
{
    int rxPtr, rxLen;
    rxBuf[0]=0;
    rxLen = busReadByte(req);

    if(rxLen == BYTE_READ_ERROR)
        return -1;

    for(rxPtr=0; rxPtr<rxLen; rxPtr++)
    {
        rxBuf[rxPtr] = busReadByte(req);

        if(rxBuf[rxPtr] == BYTE_READ_ERROR)
            return -1;

    }
    rxBuf[rxLen]=0;
    return rxLen;
}


int main(void)
{
    long j=0, t=0, b=0;
    byte i;

    byte tmp[60];
    byte rxPtr = 0;
    byte rxLen = 0;

    initBus();

    setReq(0, 0);
    setReq(1, 0);
    setReq(2, 0);
    initUart();

    ADPCFG = 0xFFFF;
    LATB = 0;
    TRISB = 0;


    initUart();

    sendString("\n\rMaster starting...\n\r");
    for(j=0; j<100000; j++);


    /*
     * Key Commands:
     * P - ping
     * I - identify
     *
     */

    while(1)
    {
        sendString("\n\r\n\r>: ");
        byte c = waitchar();
   //     sendByte(c);    /* Local Echo */

        switch(c)
        {

            case 'P':
            {
                sendString("\n\rPinging all slaves");
                for(i=0; i<NUM_SLAVES; i++)
                {
                    sprintf(tmp, "\n\rSlave #%d: ", i);
                    sendString(tmp);

                    if(busWriteByte(BUS_CMD_PING, i) != 0)
                        sendString("Comm Error: TX Timeout");
                    else
                    {
                        byte len = readDataBlock(i);

                        switch(len)
                        {
                            case 0:
                                sendString("Reply OK");
                            break;

                            case -1:
                                sendString("Comm Error: RX Timeout");
                            break;

                            default:
                                sendString("Unknown Reply");
                        }
                    }
                }

                break;
            }

            case 'I':
            {
                sendString("\n\rIdentifying slaves");
                for(i=0; i<NUM_SLAVES; i++)
                {
                    busWriteByte(BUS_CMD_ID, i);
                    byte len = readDataBlock(i);
                    sprintf(tmp, "\n\rSlave #%d replies: <", i);
                    sendString(tmp);
                    sendString(rxBuf);
                    sendString(">");
                }
                break;
            }

            case 'R':
            {
                byte id = waitchar() & 0x0F;
                byte addr = waitchar() & 0x0F;

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
                byte id = waitchar() & 0x0F;
                byte addr = waitchar() & 0x0F;
                byte val = waitchar();

                sendString("\n\rWriting config...");
                busWriteByte(BUS_CMD_WRITE_REG, id);
                busWriteByte(addr, id);
                busWriteByte(val, id);

                sprintf(tmp, "\n\rSlave #%d config register %d set to <%c>", id, addr, val);
                sendString(tmp);
                break;
            }

            default:
            {
                sendString("\n\rUnknown command: ");
                sendByte(c);
            }
        }
    }

/*
    while(1)
    {
        for(i=0; i<2; i++)
        {
            sprintf(tmp, "\n\rReading data from PIC #%d...", i);
            sendString(tmp);

            busWriteByte(0, i);
            rxLen = busReadByte(i);

            sprintf(tmp, "\n\r\tLength is %d", rxLen);
            sendString(tmp);

            for(rxPtr=0; rxPtr<rxLen; rxPtr++)
            {
                rxBuf[rxPtr] = busReadByte(i);
            }

            rxBuf[rxLen]=0;

            sendString("\n\r\tData is <");
            sendString(rxBuf);
            sendString(">\n\r");

            for(j=0; j<100000; j++);
        }

        for(j=0; j<100000; j++);
    }
*/
    while(1);
}
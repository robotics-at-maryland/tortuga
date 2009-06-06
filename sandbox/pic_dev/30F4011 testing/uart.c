/*

Motor controller connections:
It's ASCII-tacular. Use a fixed pitch font

    (Sensor Board Rev 01)
     _________________________________________
    |[USB]               ......             * |
    |                    .. ... :::::::     * |
    |..                                       |
    |.. <- MC 4 (Row 7)       . <- MC 3 (PGM) |
    |..                       :               |
    |..                                       |
    |..                                    .. |
    |..                                    .. |
    |                                      .. |
    |                                         |
    | .                                    .. |
    | :                                    .. |
    |                      (Row 3) MC 2 -> .. |
    |            : :                       .. |
    |                                      .. |
    | ** ::::::  ::      (PGM) MC 1 -> ...    |
    |_________________________________________|

MC1: JP11, pins 2 and 3 (3 = TX)
MC2: JP14, row 3 (outer = TX)
MC3: JP10, pins 2 and 3 (3 = TX)
MC4: JP14, row 3 (outer = TX)

(ie, the receive pin always is towards the middle of the board on 1-4)

WHITE = TRANSMIT


The remaining two UARTs are somewhere on JP14 and JP13. See schematic.

MC #5
JP13
Purple - Row 4, away from board
White - Row 3, towards the board


MC #6
JP14
White - Row 7, towards the board
Blue - Row 9, away from the board

*/


#define MASTER_U1_BRG 15
#define byte unsigned char

/* Motor controller addresses */
#define U1_MM_ADDR 0x01
#define U2_MM_ADDR 0x01

/* Master */
#ifdef SENSORBOARD_IC1
    /* JP13 Row 7, outer = tx */
    #define HAS_U2


    #define U2_MM_ADDR 0x04
    #define U2_BRG 11
#endif

/* LCD */
#ifdef SENSORBOARD_IC2
    /* PGD and PGC, pin 3 = tx */
    #define HAS_U1

    /* JP14 Row 3, outer = tx */
    #define HAS_U2

    #define U1_BRG 11
    #define U2_BRG 11

    #define U1_MM_ADDR 0x03
    #define U2_MM_ADDR 0x02
#endif

/* Temp */
#ifdef SENSORBOARD_IC3
    /* U1 multiplexed to I2C. U1Alt multiplexed to bus */
    /* U2 is available but TX/RX not conveliently placed */
#endif

/* Depth (slave.c) */
#ifdef SENSORBOARD_IC4
    /* PGD and PGC, pin 3 = tx */
    #define HAS_U1

                    // 48 for PLL4
    #define U1_BRG 11
    #define U1_MM_ADDR 0x01
    /* U2 available, but not conveliently placed */
#endif


#ifdef HAS_U1
    #define HAS_UART
#endif

#ifdef HAS_U2
    #define HAS_UART
#endif





#ifdef HAS_U1
/* Must be a power of 2 */
#define U1TXBUF_SIZE 32
#define U1RXBUF_SIZE 32
unsigned char U1CanRead();
unsigned char U1CanWrite();
unsigned char U1ReadByte();
void U1WriteByte(unsigned char b);

unsigned char U1TXBuffer[U1TXBUF_SIZE];
unsigned char U1RXBuffer[U1RXBUF_SIZE];
unsigned char U1TXReadPtr;
unsigned char U1TXWritePtr;
unsigned char U1TXSize;
unsigned char U1RXReadPtr;
unsigned char U1RXWritePtr;
unsigned char U1RXSize;
#endif


#ifdef HAS_U2
#define U2TXBUF_SIZE 32
#define U2RXBUF_SIZE 32
unsigned char U2CanRead();
unsigned char U2CanWrite();
unsigned char U2ReadByte();
void U2WriteByte(unsigned char b);

unsigned char U2TXBuffer[U2TXBUF_SIZE];
unsigned char U2RXBuffer[U2RXBUF_SIZE];
unsigned char U2TXReadPtr;
unsigned char U2TXWritePtr;
unsigned char U2TXSize;
unsigned char U2RXReadPtr;
unsigned char U2RXWritePtr;
unsigned char U2RXSize;

#endif


#ifdef HAS_UART
void initInterruptUarts()
{

#ifdef HAS_U1
    U1MODE = 0x0000;
    U1BRG = U1_BRG;  /* 7 for 115200 at 15 MIPS */
    U1MODEbits.ALTIO = 0;   // Use alternate IO
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;   // Enable transmit

    U1TXReadPtr=0;
    U1TXWritePtr=0;
    U1TXSize=0;
    U1RXWritePtr=0;
    U1RXReadPtr=0;
    U1RXSize=0;

    U1STAbits.UTXISEL=1;    /* Generate interrupt only when buffer is empty */
    U1STAbits.URXISEL=0;    /* Generate interrupt when a byte comes in */
    IEC0bits.U1TXIE = 1;    /* Enable TX interrupt */
    IEC0bits.U1RXIE = 1;    /* Enable RX interrupt */
#endif

#ifdef HAS_U2
    U2MODE = 0x0000;
    U2BRG = U2_BRG;  /* 7 for 115200 at 15 MIPS */
    U2MODEbits.ALTIO = 0;   // Use alternate IO
    U2MODEbits.UARTEN = 1;
    U2STAbits.UTXEN = 1;   // Enable transmit

    U2TXReadPtr=0;
    U2TXWritePtr=0;
    U2TXSize=0;
    U2RXWritePtr=0;
    U2RXReadPtr=0;
    U2RXSize=0;

    U2STAbits.UTXISEL=1;    /* Generate interrupt only when buffer is empty */
    U2STAbits.URXISEL=0;    /* Generate interrupt when a byte comes in */
    IEC1bits.U2TXIE = 1;    /* Enable TX interrupt */
    IEC1bits.U2RXIE = 1;    /* Enable RX interrupt */
#endif
}
#endif


#ifdef HAS_U1
void _ISR _U1RXInterrupt(void)
{
    IFS0bits.U1RXIF = 0;    /* Clear RX interrupt */
    U1STAbits.OERR = 0;
    U1STAbits.FERR = 0;
    U1STAbits.PERR = 0;
    U1STAbits.URXDA = 0;

    while(U1STAbits.URXDA == 1)
    {
        U1RXBuffer[U1RXWritePtr] = U1RXREG;
        U1RXWritePtr = (U1RXWritePtr + 1) & (U1RXBUF_SIZE-1);

        if(U1RXSize < U1RXBUF_SIZE-1)
            U1RXSize++;
    }
}

void _ISR _U1TXInterrupt(void)
{
    IFS0bits.U1TXIF = 0;    /* Clear TX interrupt */
    while(U1STAbits.UTXBF == 0 && U1TXSize > 0)
    {
        U1TXREG = U1TXBuffer[U1TXReadPtr];
        U1TXReadPtr = (U1TXReadPtr+1) & (U1TXBUF_SIZE-1);
        U1TXSize--;
    }
}

unsigned char U1CanWrite()
{
    return U1TXSize < (U1TXBUF_SIZE-1);
}

unsigned char U1CanRead()
{
    return U1RXSize;
}

void U1WriteByte(unsigned char b)
{
    IEC0bits.U1TXIE = 0;    /* Disable TX interrupt */
    U1TXBuffer[U1TXWritePtr] = b;
    U1TXWritePtr = (U1TXWritePtr+1) & (U1TXBUF_SIZE-1);
    U1TXSize++;
    IEC0bits.U1TXIE = 1;    /* Enable TX interrupt */
    IFS0bits.U1TXIF = 1;    /* Force TX interrupt */
}

unsigned char U1ReadByte()
{
    unsigned char b;
    IEC0bits.U1RXIE = 0;    /* Disable RX interrupt */
    U1RXSize--;
    b = U1RXBuffer[U1RXReadPtr];
    U1RXReadPtr = (U1RXReadPtr+1) & (U1RXBUF_SIZE-1);
    IEC0bits.U1RXIE = 1;    /* Enable RX interrupt */
    return b;
}

void U1ClearRXBuffer()
{
    IEC0bits.U1RXIE = 0;    /* Disable RX interrupt */
    U1RXSize=0;
    U1RXReadPtr = U1RXWritePtr;
    IEC0bits.U1RXIE = 1;    /* Enable RX interrupt */
}

#endif

#ifdef HAS_U2
void _ISR _U2RXInterrupt(void)
{
    IFS1bits.U2RXIF = 0;    /* Clear RX interrupt */
    U2STAbits.OERR = 0;
    U2STAbits.FERR = 0;
    U2STAbits.PERR = 0;
    U2STAbits.URXDA = 0;

    while(U2STAbits.URXDA == 1)
    {
        U2RXBuffer[U2RXWritePtr] = U2RXREG;
        U2RXWritePtr = (U2RXWritePtr + 1) & (U2RXBUF_SIZE-1);
        if(U2RXSize < U2RXBUF_SIZE-1)
            U2RXSize++;
    }
}

void _ISR _U2TXInterrupt(void)
{
    IFS1bits.U2TXIF = 0;    /* Clear TX interrupt */
    while(U2STAbits.UTXBF == 0 && U2TXSize > 0)
    {
        U2TXREG = U2TXBuffer[U2TXReadPtr];
        U2TXReadPtr = (U2TXReadPtr+1) & (U2TXBUF_SIZE-1);
        U2TXSize--;
    }
}

unsigned char U2CanWrite()
{
    return U2TXSize < (U2TXBUF_SIZE-1);
}

unsigned char U2CanRead()
{
    return U2RXSize;
}

void U2WriteByte(unsigned char b)
{
    IEC1bits.U2TXIE = 0;    /* Disable TX interrupt */
    U2TXBuffer[U2TXWritePtr] = b;
    U2TXWritePtr = (U2TXWritePtr+1) & (U2TXBUF_SIZE-1);
    U2TXSize++;
    IEC1bits.U2TXIE = 1;    /* Enable TX interrupt */
    IFS1bits.U2TXIF = 1;    /* Force TX interrupt */
}

unsigned char U2ReadByte()
{
    unsigned char b;
    IEC1bits.U2RXIE = 0;    /* Disable RX interrupt */
    U2RXSize--;
    b = U2RXBuffer[U2RXReadPtr];
    U2RXReadPtr = (U2RXReadPtr+1) & (U2RXBUF_SIZE-1);
    IEC1bits.U2RXIE = 1;    /* Enable RX interrupt */
    return b;
}

void U2ClearRXBuffer()
{
    IEC1bits.U2RXIE = 0;    /* Disable RX interrupt */
    U2RXSize=0;
    U2RXReadPtr = U2RXWritePtr;
    IEC1bits.U2RXIE = 1;    /* Enable RX interrupt */
}

#endif



#ifdef HAS_UART
void UARTWriteByte(byte b, byte destUart)
{
    if(destUart == 0)
    {
        #ifdef HAS_U1
        U1WriteByte(b);
        #endif
    }
    else
    {
        #ifdef HAS_U2
        U2WriteByte(b);
        #endif
    }
}

void UARTSendSpeed(byte addr, byte msb, byte lsb, byte destUart)
{
    if(destUart == 0)
    {
        #ifdef HAS_U1
        U1ClearRXBuffer();
        #endif
    }
    else
    {
        #ifdef HAS_U2
        U2ClearRXBuffer();
        #endif
    }
    UARTWriteByte(0x14, destUart); /* Set speed  */
    UARTWriteByte(addr, destUart); /* MM address */
    UARTWriteByte(lsb, destUart);
    UARTWriteByte(msb, destUart);
    UARTWriteByte((0x14 + addr + msb + lsb) & 0xFF, destUart);
}
#endif


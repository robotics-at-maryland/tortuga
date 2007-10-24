#define IC1_U1_BRG 1
#define IC1_U2_BRG 1

//#define HAS_U2

#ifdef SENSORBOARD_IC1
    #define HAS_U2
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

#endif

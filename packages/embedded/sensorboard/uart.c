#include <p30fxxxx.h>
#include "buscodes.h"
#include "uart.h"

unsigned char U2TXBuffer[U2TXBUF_SIZE];
unsigned char U2RXBuffer[U2RXBUF_SIZE];
unsigned char U2TXReadPtr;
unsigned char U2TXWritePtr;
unsigned char U2TXSize;
unsigned char U2RXReadPtr;
unsigned char U2RXWritePtr;
unsigned char U2RXSize;


void _ISR _U2RXInterrupt(void)
{
    IFS1bits.U2RXIF = 0;    /* Clear RX interrupt */
    U1STAbits.OERR = 0;
    U1STAbits.FERR = 0;
    U1STAbits.PERR = 0;
    U1STAbits.URXDA = 0;

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

/* Must be a power of 2 */
#define U2TXBUF_SIZE 32
#define U2RXBUF_SIZE 32

unsigned char U2CanRead();
unsigned char U2CanWrite();

unsigned char U2ReadByte();
void U2WriteByte(unsigned char b);
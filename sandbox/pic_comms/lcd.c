#include <p30fxxxx.h>
#include <string.h>

//_FOSC( CSW_FSCM_OFF & FRC );
_FOSC( CSW_FSCM_OFF & FRC );
//_FOSC( FRC_LO_RANGE);
//_FOSCSEL(FRC);
//_FPOR( PWRT_OFF);
//Turn off the watchdog
_FWDT ( WDT_OFF );

#define TRIS_OUT 0
#define TRIS_IN  1
#define byte unsigned char

#define LAT_RS _LATB5
#define LAT_E  _LATB4


void lcdPulse()
{
    long i;
    LAT_E = 1;
    for(i=0; i<300; i++);
    LAT_E = 0;
    for(i=0; i<300; i++);
}


void lcdWrite(byte b)
{
    LATB = (LATB & 0xFFF0) | (b&0x0F);
}


void lcdByte(byte b)
{
    lcdWrite( (b & 0xF0) >> 4);
    lcdPulse();
    lcdWrite(b & 0x0F);
    lcdPulse();
}

void lcdInit()
{
    long i=0;
    ADPCFG = 0xFFFF;
    LATB = 0;
    TRISB = 0;


    lcdWrite (0x00);
    for(i=0; i<100000; i++);
    LAT_RS = 0;
    lcdWrite(0x03);   /* init with specific nibbles to start 4-bit mode */

    lcdPulse();
    lcdPulse();
    lcdPulse();

    lcdWrite(0x02);
    lcdPulse();
    lcdByte(0x2C);    /* function set (all lines, 5x7 characters) */

    //lcdByte(0x0C);    /* display ON, cursor off, no blink */
    lcdByte(0x0F);

    lcdByte(0x01);    /* clear display */
    lcdByte(0x06);    /* entry mode set, increment & scroll left */
    LAT_RS = 1;
}

void lcdCmd(byte b)
{
    LAT_RS = 0;
    lcdByte(b);
    LAT_RS = 1;
}

void lcdChar(byte b)
{
    lcdByte(b);
}



void main()
{
    byte i;
    byte data[] = "Hello World     ";

    lcdInit();
    for(i=0; i<16; i++)
        lcdChar(data[i]);

    while(1);
}
//#include <stdio.h>
//#include "p30f1010.h"

///////////////////////////////////////////////
//ORIGINAL STUFF
///////////////////////////////////////////////


/* Device configuration register macros for building the hex file */
//_FOSC(EC & HS);          
//_FOSCSEL(PRIOSC_PLL);
//_FWDT ( WDT_OFF );
////_FPOR(0xFFF7) //turning off MCLR pin?


/*#define byte unsigned char

void initUart()
{
    U1MODE = 0x0000;
    U1BRG = 832;
    U1MODEbits.ALTIO = 1;   // Use alternate IO
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;   // Enable transmit
}


void sendByte(byte i)
{
 //   U1STAbits.OERR = 0;
 //   U1STAbits.FERR = 0;
  //  U1STAbits.PERR = 0;
  //  U1STAbits.URXDA = 0;
    while(U1STAbits.UTXBF);
    U1TXREG = i;
    while(U1STAbits.UTXBF);
}

void sendString(unsigned char str[])
{
    byte i=0;
    for(i=0; str[i]!=0; i++)
        sendByte(str[i]);
}

int main(void)
{
    long i=0, j=0, t=0, b=0;
    initUart();

    while(1)
    {
        sendString("\n\revil wombat says hello");
        for(j=0; j<100000; j++);
    }

    while(1);
}
*/





/*
void main()
{
	ADPCFG = 0xFFFF;
	TRISB = 0;
	LATB = 0xFFFF;

	long i=0;
	while(1)
	{
		for(i=0; i<65000; i++);
		for(i=0; i<65000; i++);
		for(i=0; i<65000; i++);
		LATB ^= 0xFFFF;
	}

}
*/

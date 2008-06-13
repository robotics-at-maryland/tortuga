#include <p30f4011.h>
#include <stdio.h>
#include "uart.c"
#include <time.h>

//_FOSC(CSW_FSCM_OFF & ECIO);
_FOSC(CSW_FSCM_OFF & XT_PLL8);//for 30 mips on 15mHz clock 
_FWDT ( WDT_OFF );
_FOSCSEL(PRIOSC_PLL);
_FGS(CODE_PROT_OFF);

//#define byte unsigned char

/* UART BRG "Baud" rate calculation "How To"
Baud Rate is the same as Bits Per Second
	U1BRG = (MIPS / BAUD / 16) - 1
EXAMPLE...
15MHz Oscillator with pll of 8
MIPS = 15MMhz * pll8 / 4 = 30,000,000
30000000/BAUD(230400) = 130.208...
130.208... / 16 - 1 = 7.138... ~~~ 7
****don't forget to invert the signal coming out of the PIC
to create a signal that most computer's serial ports will 
interpret  */
void initMasterUart()
{
    U1MODE = 0x0000;
	U1BRG = MASTER_U1_BRG;  //15 for 115200 at 30 mips
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

/*void DelayNmSec(int x){
	clock_t start = clock();
	clock_t end = x * CLOCKS_PER_SEC / 1000;
	while (clock() - start < end);
}*/

void delay(int i){
	int x;
	for (x=0;x<i;x++);
}

int main(void){
	_TRISC13 = 0;
	byte ADCValue;
	initMasterUart();
	ADPCFG = 0xFFF7; // all PORTB = Digital; RB2 = analog (wrong)
	ADCON1 = 0x0000; // SAMP bit = 0 ends sampling ...
	// and starts converting
	ADCHS = 0x0003; // Connect RB2/AN2 as CH0 input ..
	// in this example RB2/AN2 is the input (wrong)
	ADCSSL = 0;
	ADCON3 = 0x000F; // Manual Sample, Tad = internal 2 Tcy
	ADCON2 = 0;
	ADCON1bits.ADON = 1; // turn ADC ON
	int i;
	while (1){
		//delay(30000);
		ADCON1bits.SAMP = 1; // start sampling ...
		//DelayNmSec(1); // for 100 mS
		ADCON1bits.SAMP = 0; // start Converting
		while (!ADCON1bits.DONE); // conversion done?
		ADCValue = ADCBUF0>>2; // yes then get ADC value
		sendByte(ADCValue);
		/*for(i=0;i<6;i++){
			sendByte('@');
		}*/
	}
	return 0;
}

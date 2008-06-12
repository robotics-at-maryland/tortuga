#include <p30f4011.h>
#include <stdio.h>
#include "uart.c"
#include <time.h>

_FOSC(CSW_FSCM_OFF & ECIO); 
_FWDT ( WDT_OFF );
_FOSCSEL(PRIOSC_PLL);
_FGS(CODE_PROT_OFF);

#define byte unsigned char

void initMasterUart()
{
    U1MODE = 0x0000;
	//U1BRG = 15;  /* 7 for 230400, 15 for 115200 194 for 9600  AT 30 MIPS*/
    U1BRG = MASTER_U1_BRG;  /* 7 for 115200 at 15 MIPS */
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

void DelayNmSec(int x){
	clock_t start = clock();
	clock_t end = x * CLOCKS_PER_SEC / 1000;
	while (clock() - start < end);
}

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
	int i, end;
	while (1){
		delay(30000);
		ADCON1bits.SAMP = 1; // start sampling ...
		DelayNmSec(1); // for 100 mS
		ADCON1bits.SAMP = 0; // start Converting
		while (!ADCON1bits.DONE); // conversion done?
		ADCValue = ADCBUF0>>2; // yes then get ADC value
		sendByte(ADCValue);
	}
	return 0;
}








#include "SlidingDFT.h"





void InitSlidingDFT(void)
{
	// Set up TMR89 for timebase operation
	
	TMR8 = 0;
	TMR9 = 0;
	
	T8CON = 0x8024;
	
	
	// Set up the ADC to collect data
	
	// Fosc = 80Mhz
	
	
	AD1PCFGH = 0xFFFF;		// AN31-16 digital mode
	AD1PCFGL = 0xFFF3;		// AN3 analog input
	_TRISB3 = 1;
	_TRISB2 = 1;
	
	AD1CSSH = 0x0000;
	AD1CSSL = 0x0008;
	AD1CHS0 = 0x0003;		// select AN3 for everything
	
	AD1CON1 = 0x17E4;
	AD1CON2 = 0x0006;
	AD1CON3 = 0x03FF;		// Tad = 1us, Tsam = 2us
	AD1CON4 = 0x0000;
	
	// Enable the ADC in free running mode at some undetermined frequency
	AD1CON1 |= 0x8000;
	
	
	
	
	
	
	
	
}





#include <p33fxxxx.h>
#include <dsp.h>


#include "SlidingDFT.h"

#define NUM_CHANNELS 4
#define NUM_SAMPLES	128

#pragma udata BigMemory
//fractional __attribute__((address(0x2000))) Data[4096];

signed long window[2*NUM_CHANNELS*NUM_SAMPLES];

signed long sums[2*NUM_CHANNELS];

fractional __attribute__((space(xmemory))) data[NUM_CHANNELS];

fractcomplex coefficients[NUM_SAMPLES];

#pragma code

	
int main()
{
	
	OSCCON = 0x0000;		// unlock clock settings
	CLKDIV = 0x0000;		// post divide-by factors set to /2
	PLLFBD = 43;			// puts clock speed at 79.2576Mhz
	
	
	
	asmInitDFT(coefficients,window,sums,NUM_SAMPLES,NUM_CHANNELS);
	
	while(1) {
		asmUpdateDFT(data);
	}
	
	
/*	
	
	
	
	fractional trigger = 0x400;
	fractcomplex wFactor;
	wFactor.real = 0x65DE;
	wFactor.imag = 0x4D81;
	
	unsigned int Index;
	unsigned long BufferTime;
	unsigned long Timestamps[4];
	
	unsigned int Return;
	
	
	InitSlidingDFT();
	
	
	TRISD = 0x0000;
	TRISG = 0x0000;
	LATD = 0x0000;
	
		
	while(1)
	{
	//	LATD = ADC1BUF0>>8;
	//	_LATG15 = 1;
	//	_LATG15 = 0;
	//	while(_AD1IF==0);
	//	_AD1IF=0;
	Return = asmSlidingDFT2( 1024, 1024, 34, trigger, wFactor, Data, &Index, &BufferTime, Timestamps, 20000 );
	
	LATD = Return<<4;

	}
	
	*/
	
	return 0;
}


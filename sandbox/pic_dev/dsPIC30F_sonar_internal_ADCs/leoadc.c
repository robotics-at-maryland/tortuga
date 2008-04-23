#include <p30fxxxx.h>
#include "scottcommon.h"


#define M_SLIDING_DFT_k 4
#define M_SLIDING_DFT_N 32
#define M_SLIDING_DFT_nchannels 4
#include <dft_singleton_c30.h>


#define DFT_MAG_THRESHOLD 1000
sliding_dft_t *dft;


void reportPing(void)
{
	byte *buf, *end;
	
	buf = (byte*)(dft->re);
	end = buf + sizeof(adcmath_t) * M_SLIDING_DFT_nchannels;
	
	for (; buf < end ; buf ++)
		sendByte(*buf);
	
	buf = (byte*)(dft->im);
	end = buf + sizeof(adcmath_t) * M_SLIDING_DFT_nchannels;
	
	for (; buf < end ; buf ++)
		sendByte(*buf);
}


void __attribute__((interrupt, auto_psv)) _ISR _ADCInterrupt(void)
{
	int i;
	
	//	Copy and cast converted ADC data to the correct type
	adcdata_t rawData[M_SLIDING_DFT_nchannels];
	for (i = 0 ; i < M_SLIDING_DFT_nchannels ; i ++)
		rawData[i] = (&ADCBUF0)[i];
	
	//	Propate the DFT
	dft_update(dft, rawData);
	
	int thresholdMet = 1;
	for (i = 0 ; i < M_SLIDING_DFT_nchannels ; i ++)
		thresholdMet &= (dft->mag[i] > DFT_MAG_THRESHOLD);
	
	if (thresholdMet)
		reportPing();
	
	//	Clear interrupt bit; we are done servicing the interrupt
	IFS0bits.ADIF = 0;
}


int main(void)
{
	//	Initialize the sliding DFT
	dft = M_DFT_INIT();
	
	
	//	Initialize UART
	init_Uart(1);
	
	
	//	Initilize ADCs for simultaneous sampling and sequential conversion 
	//	on four channels
	
	ADPCFG = 0xFF78;	// RB0,RB1,RB2 & RB7 = analog 
	ADCON1 = 0x00EC;	// SIMSAM bit = 1 implies ... 
						// simultaneous sampling 
						// ASAM = 1 for auto sample after convert 
						// SSRC = 111 for 3Tad sample time 
	ADCHS = 0x0007;		// Connect AN7 as CH0 input 
	ADCSSL = 0;
	ADCON3 = 0x0302;	// Auto Sampling 3 Tad, Tad = internal 2 Tcy 
	ADCON2 = 0x030C;	// CHPS = 1x implies simultaneous ... 
						// sample CH0 to CH3
						// SMPI = 0011 for interrupt after 4 converts 
	
	IFS0bits.ADIF = 0;  // Clear the A/D interrupt flag bit
	IEC0bits.ADIE = 1;  // Set the A/D interrupt enable bit
	
	ADCON1bits.ADON = 1;// turn ADC ON
	
	
	while (1);          // run indefinitely
}

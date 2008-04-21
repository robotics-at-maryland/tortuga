#include <p30f4011.h>

#define M_SLIDING_DFT_k 5
#define M_SLIDING_DFT_N 10
#define M_SLIDING_DFT_nchannels 4
#include <dft_singleton_c30.h>

int main(void)
{
	sliding_dft_t *dft = M_DFT_INIT();
	adcdata_t rawData[M_SLIDING_DFT_nchannels];
	int i;
	
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
	
	ADCON1bits.ADON = 1;// turn ADC ON 
	while (1) // repeat continuously 
	{ 
		//ADC16Ptr = &ADCBUF0;		// initialize ADCBUF pointer 
		//OutDataPtr = &OutData[0];	// point to first TXbuffer value 
		IFS0bits.ADIF = 0;			// clear interrupt 
		while (IFS0bits.ADIF);		// conversion done?
		for (i = 0 ; i < M_SLIDING_DFT_nchannels ; i ++)
			rawData[i] = (&ADCBUF0)[i];
		dft_update(dft, rawData);
	} // repeat
}

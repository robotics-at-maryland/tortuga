


#ifndef SLIDING_DFT_H
#define SLIDING_DFT_H


#include <p33fxxxx.h>
#include <dsp.h>
#include <timer.h>


#define DFT_FRACTIONAL		0x1000
#define DFT_INTEGER			0x1001
#define DFT_SIGNED			0x0001
#define DFT_UNSIGNED		0x1001



/*extern int asmSlidingDFT2( unsigned int N, unsigned int M, unsigned int P, 
							fractional trigger, fractcomplex wFactor, 
							fractional * pData, int * pIndex, 
							unsigned long * pBufferTime, unsigned long * pTimestamps, 
							unsigned long Timeout );
*/
extern void asmUpdateDFT( fractional * sample );
extern void asmInitDFT( fractcomplex * coeff_buffer, 
						signed long * window_buffer, 
						signed long * sum_buffer, 
						unsigned int buffer_length, 
						unsigned int num_channels,
						unsigned int math_mode );
extern void asmExitDFT(void);



void InitSlidingDFT(void);


#endif


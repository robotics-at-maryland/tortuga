


#ifndef SLIDING_DFT_H
#define SLIDING_DFT_H


#include <p33fxxxx.h>
#include <dsp.h>
#include <timer.h>



extern int asmSlidingDFT2( unsigned int N, unsigned int M, unsigned int P, 
							fractional trigger, fractcomplex wFactor, 
							fractional * pData, int * pIndex, 
							unsigned long * pBufferTime, unsigned long * pTimestamps, 
							unsigned long Timeout );

extern void asmUpdateDFT( fractional * sample );
extern void asmInitDFT( fractcomplex * coeff_buffer, 
						signed long * window_buffer, 
						signed long * sum_buffer, 
						unsigned int buffer_length, 
						unsigned int num_channels );



void InitSlidingDFT(void);


#endif


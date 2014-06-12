/**
 * Simple sonar car demo (phase based TDOAs)
 *
 * PHYSICAL LAYOUT
 * Arrange the piezos in an L-shape, with the legs 5 centimeters long.
 * The piezo in the center of the L should be channel 0 on the DFT.
 * Whichever piezo you choose to be channel 1 should point forward on the car.
 * Channel 2 should point to the side.
 *
 * Once the car is properly responding to pings, you may wish to mount the
 * array at a slight angle relative to the car's body.  Depending on the angle
 * you select, you can make the car either head straight for sound sources
 * or try to spiral in toward them at varying rates.
 *
 * TODO
 * Sections marked TODO must be filled in with project specific code.
 *
 * The allStop() and steerToward() routines should be non-blocking.
 * In other words, they should adjust the peripheral settings to tweak the 
 * motor speeds and return immediately.
 */

#include <p30fxxxx.h>
#include <string.h>
#include <stdio.h>

//  Configure sliding DFT for 3 kHz, 3 channels
#define M_SLIDING_DFT_k 4
#define M_SLIDING_DFT_N 32
#define M_SLIDING_DFT_nchannels 4
#include <Sonar.h>



//	Statically allocate the necessary arrays
adcdata_t coefreal[M_SLIDING_DFT_nchannels], coefimag[M_SLIDING_DFT_nchannels];
adcmath_t windowreal[M_SLIDING_DFT_nchannels][M_SLIDING_DFT_N], windowimag[M_SLIDING_DFT_nchannels][M_SLIDING_DFT_N];
adcmath_t sumreal[M_SLIDING_DFT_nchannels], sumimag[M_SLIDING_DFT_nchannels], mag[M_SLIDING_DFT_nchannels];
adcdata_t sample[M_SLIDING_DFT_nchannels];


int curidx;


void dft_init();
void dft_update();
void dft_setup_coefficients();
void dft_setup_window();
void dft_purge();
float dft_relativePhase(int channelA, int channelB);


adcmath_t adcabs(adcmath_t i)
{
	if (i <= 0)
		return -i;
	else
		return i;
}


void dft_init()
{
	dft_setup_coefficients();
	dft_setup_window();
}


/** Update the one selected Fourier amplitude using a sliding DFT
 */
void dft_update()
{
	int channel;
	for (channel = 0 ; channel < M_SLIDING_DFT_nchannels ; channel ++)
	{
		/*	We subtract window____[channel][firstidx] because both windowreal[i]
		 *	and windowimag[i] are circular buffers.  On the next call of this 
		 *	function, window____[channel][firstidx] will be overwritten with 
		 *	computations from the next sample.
		 */
		
		sumreal[channel] -= windowreal[channel][curidx];
		sumimag[channel] -= windowimag[channel][curidx];
		
		/*	For each sample we receive, we only need to compute one new term in
		 *	the DFT sum.  These two lines together compute 
		 *
		 *		f(N-1) x cos(2 pi k (N - 1) / N) 
		 *
		 *	and
		 *
		 *		f(N-1) x sin(2 pi k (N - 1) / N)
		 *
		 *	which are, respectively, the real and imaginary parts of 
		 *	
		 *		f(N-1) x exp(2 pi i k (N - 1) / N)
		 *	
		 *	which is simply the last term of the sum for F(k).
		 */
		
		windowreal[channel][curidx] = (adcmath_t) coefreal[curidx] * sample[channel];
		windowimag[channel][curidx] = (adcmath_t) coefimag[curidx] * sample[channel];
		
		/*	The next two lines update the real and imaginary part of the complex
		 *	output amplitude.
		 *	
		 *	We add window____[channel][curidx] to sum____[chan constnel] because all 
		 *	of the previous N-1 terms of F(k), numbered 0,1,...,N-3,N-2, were 
		 *	calculated and summed during previous iterations of this function - 
		 *	hence the name "sliding DFT".
		 */
		
		sumreal[channel] += windowreal[channel][curidx];
		sumimag[channel] += windowimag[channel][curidx];
		
		/*	We compute the L1 norm (|a|+|b|) instead of the L2 norm 
		 *	sqrt(a^2+b^2) in order to aovid integer overflow.  Since we are only
		 *	using the magnitude for thresholding, this is an acceptable 
		 *	approximation.
		 */
		
		mag[channel] = adcabs(sumreal[channel]) + adcabs(sumimag[channel]);
	}
	
	/*	curidx represents the index into the circular buffers 
	 *	windowreal[channel] and windowimag[channel] at which the just-received
	 *	sample will be added to the DFT sum.
	 */
	
	++curidx;
	if (curidx == M_SLIDING_DFT_N)
		curidx = 0;
}


void dft_setup_coefficients()
{
	int n;
	for (n = 0 ; n < M_SLIDING_DFT_N ; n++)
	{
		coefreal[n] = (adcdata_t) (cos(- 2 * M_PI * n * M_SLIDING_DFT_k / M_SLIDING_DFT_N) * ADCDATA_MAXAMPLITUDE);
		coefimag[n] = (adcdata_t) (sin(- 2 * M_PI * n * M_SLIDING_DFT_k / M_SLIDING_DFT_N) * ADCDATA_MAXAMPLITUDE);
	}
}


void dft_setup_window() {
	dft_purge();
}


void dft_purge()
{
	int i;
	memset(sumreal, 0, sizeof(*sumreal) * M_SLIDING_DFT_nchannels);
	memset(sumimag, 0, sizeof(*sumimag) * M_SLIDING_DFT_nchannels);
	memset(mag, 0, sizeof(*mag) * M_SLIDING_DFT_nchannels);
	for (i = 0 ; i < M_SLIDING_DFT_nchannels ; i ++)
	{
		memset(windowreal[i], 0, sizeof(**windowreal) * M_SLIDING_DFT_N);
		memset(windowimag[i], 0, sizeof(**windowimag) * M_SLIDING_DFT_N);
	}
	curidx = 0;
}



void resetTimer(void)
{
	T1CONbits.TON = 0;
	TMR1 = 0;
	PR1 = 0xFFFF;
	T1CONbits.TSIDL = 0;
	T1CONbits.TCKPS = 0;
	T1CONbits.TCS = 0;
}



int main(void)
{
	int i;
	const int numDFTs0 = 1;
	const int numDFTs1 = 10;
	const int numDFTs2 = 100;
    dft_init();  //  Initialize the sliding DFT
    
	printf("With %d channels, k=%d, N=%d:\n", M_SLIDING_DFT_nchannels, M_SLIDING_DFT_k, M_SLIDING_DFT_N);
	
	memset(sample, 0, sizeof(adcdata_t) * M_SLIDING_DFT_nchannels);
	
	resetTimer();
	T1CONbits.TON = 1;
	for (i = 0 ; i < numDFTs0 ; i ++)
		dft_update();
	T1CONbits.TON = 0;
	
	printf("  Ran %d DFTs in %d clock cycles.\n", numDFTs0, TMR1);
	printf("    %d cycles per channel\n", (int)((float)TMR1/(M_SLIDING_DFT_nchannels*numDFTs0)));
	
	resetTimer();
	T1CONbits.TON = 1;
	for (i = 0 ; i < numDFTs1 ; i ++)
		dft_update();
	T1CONbits.TON = 0;
	
	printf("  Ran %d DFTs in %d clock cycles.\n", numDFTs1, TMR1);
	printf("    %d cycles per channel\n", (int)((float)TMR1/(M_SLIDING_DFT_nchannels*numDFTs1)));
	
	resetTimer();
	T1CONbits.TON = 1;
	for (i = 0 ; i < numDFTs2 ; i ++)
		dft_update();
	T1CONbits.TON = 0;
	
	printf("  Ran %d DFTs in %d clock cycles.\n", numDFTs2, TMR1);
	printf("    %d cycles per channel\n", (int)((float)TMR1/(M_SLIDING_DFT_nchannels*numDFTs2)));
	
	return 0;
}

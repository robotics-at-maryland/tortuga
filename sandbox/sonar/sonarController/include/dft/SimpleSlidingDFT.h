/**
 * @file SimpleSlidingDFT.h
 *
 * @author Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 *  
 * Simple, well documented implementation of a sliding DFT.
 * 
 */

#ifndef _RAM_SONAR_SIMPLESLIDINGDFT_H
#define _RAM_SONAR_SIMPLESLIDINGDFT_H

#include "../Sonar.h"
#include "SlidingDFT.h"
#include <strings.h>

namespace ram {
namespace sonar {


template<int nchannels, int k, int N>
class SimpleSlidingDFT : public SlidingDFT {
public:
	SimpleSlidingDFT()
	{
		for (int n = 0 ; n < N ; n++)
		{
			coefreal[n] = (adcdata_t) (cos(- 2 * M_PI * n * k / N) * ADCDATA_MAXAMPLITUDE);
			coefimag[n] = (adcdata_t) (sin(- 2 * M_PI * n * k / N) * ADCDATA_MAXAMPLITUDE);
		}
		purge();
	}
	
	
	virtual void purge()
	{
		bzero(sumreal, sizeof(adcmath_t) * nchannels);
		bzero(sumimag, sizeof(adcmath_t) * nchannels);
		bzero(mag, sizeof(adcmath_t) * nchannels);
		bzero(windowreal, sizeof(adcmath_t) * N * nchannels);
		bzero(windowimag, sizeof(adcmath_t) * N * nchannels);
		curidx = 0;
	}
	
	
	virtual void update(const adcdata_t * sample)
	{
		for (int channel = 0 ; channel < nchannels ; channel ++)
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
			
			mag[channel] = abs(sumreal[channel]) + abs(sumimag[channel]);
		}
		
		/*	curidx represents the index into the circular buffers 
		 *	windowreal[channel] and windowimag[channel] at which the just-received
		 *	sample will be added to the DFT sum.
		 */
		
		++curidx;
		if (curidx == N)
			curidx = 0;
	}
	
	
	virtual adcmath_t getMagL1(int channel) const {return mag[channel];}
	virtual adcmath_t getReal(int channel) const {return sumreal[channel];}
	virtual adcmath_t getImag(int channel) const {return sumimag[channel];}
	virtual int getCountChannels() const {return nchannels;}
	virtual int getFourierIndex() const {return k;}
	virtual int getWindowSize() const {return N;}
	
private:
	adcdata_t coefreal[N], coefimag[N];
	adcmath_t windowreal[nchannels][N], windowimag[nchannels][N];
	adcmath_t sumreal[nchannels], sumimag[nchannels], mag[nchannels];
	int curidx;
	
};


} // namespace sonar
} // namespace ram


#endif

/**
 * @file SDFTSpectrum.h
 * Sliding DFT based spectrum analyzer, inspired by
 * http://www.comm.toronto.edu/~dimitris/ece431/slidingdft.pdf
 *
 * @author Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 * 
 */

#ifndef _RAM_SDFT_SPECTRUM_H
#define _RAM_SDFT_SPECTRUM_H


#include "Spectrum.h"
#include <cmath>
#include <string.h>

namespace ram {
namespace sonar {

template<int N, int nchannels>
class SDFTSpectrum {
private:
	int idx;
	int16_t data[N][nchannels];
	std::complex<int64_t> fourier[N][nchannels];
	std::complex<int32_t> coef[N];
public:
	SDFTSpectrum()
	{
		//	Sample cosine and sine and store as signed 16 bit integers
		int prefactor = 1 << 15;
		for (int k = 0 ; k < N ; k ++)
		{
			coef[k].real() = prefactor * std::cos(2*M_PI*(double)k/N);
			coef[k].imag() = prefactor * std::sin(2*M_PI*(double)k/N);
		}
		purge();
	}
	
	void purge()
	{
		bzero(data, sizeof(int16_t) * N * nchannels);
		bzero(fourier, sizeof(std::complex<int64_t>) * N);
		idx = 0;
	}
	
	void update(const int16_t *sample)
	{
		//	Slide through circular buffers
		++idx;
		if (idx == N)
			idx = 0;
		
		//	Compute x[n+1]-x[n], then store new samples over old ones
		int32_t diff[nchannels];
		for (int channel = 0 ; channel < nchannels ; channel ++)
		{
			diff[channel] = sample[channel] - data[idx][channel];
			data[idx][channel] = sample[channel];
		}
		
		for (int k = 0 ; k < N ; k ++)
		{
			//	Make some convenient shorthands for numbers we need
			int32_t coefRe = coef[k].real();
			int32_t coefIm = coef[k].imag();
			
			for (int channel = 0 ; channel < nchannels ; channel ++)
			{
				int64_t &fourRe = fourier[k][channel].real();
				int64_t &fourIm = fourier[k][channel].imag();
				
				int64_t rhsRe = fourRe + diff[channel];
				
				fourRe = coefRe * rhsRe - coefIm * fourIm;
				fourIm = coefRe * fourIm + coefIm * rhsRe;
				
				fourRe = (fourRe >> 15);
				fourIm = (fourIm >> 15);
			}
		}
	}
	
	std::complex<int64_t> getAmplitude(int k, int channel) const
	{
		return fourier[k][channel];
	}
};


} // namespace sonar
} // namespace ram


#endif

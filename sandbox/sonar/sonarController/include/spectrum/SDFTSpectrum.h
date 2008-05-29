/**
 * @file SlidingDFT.h
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
	adcdata_t data[N][nchannels];
	std::complex<adcmath_t> fourier[N][nchannels];
public:
	SDFTSpectrum() { purge(); }
	void purge()
	{
		bzero(data, sizeof(adcdata_t) * N * nchannels);
		bzero(fourier, sizeof(std::complex<adcmath_t>) * N);
		idx = 0;
	}
	void update(const adcdata_t *sample)
	{
		++idx;
		if (idx == N)
			idx = 0;
		
		for (int k = 0 ; k < N ; k ++)
		{
			for (int channel = 0 ; channel < nchannels ; channel ++)
			{
				std::complex<adcmath_t> &X = fourier[k][channel];
				std::complex<adcmath_t> coef;
				coef.real() = (1 << 16) * std::cos(2*M_PI*k/N);
				coef.imag() = (1 << 16) * std::sin(2*M_PI*k/N);
				X = coef *
				(X - (adcmath_t) (data[idx][channel]) + (adcmath_t) (sample[channel])r);
				X.real() = (X.real() >> 16) & 0xFFFF;
				X.imag() = (X.imag() >> 16) & 0xFFFF;
			}
		}
	}
	std::complex<adcmath_t> getAmplitude(int k, int channel) const
	{ return fourier[k][channel]; }
};


} // namespace sonar
} // namespace ram


#endif

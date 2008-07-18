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

template<int bitDepth, int N, int nchannels>
class SDFTSpectrum : Spectrum<bitDepth> {
private:
	int idx;
	typename adctype<bitDepth>::SIGNED data[N][nchannels];
	std::complex<typename adctype<bitDepth>::QUADRUPLE_PRECISION::SIGNED> fourier[N][nchannels];
	std::complex<typename adctype<bitDepth>::SIGNED> coef[N];
public:
	SDFTSpectrum()
	{
		//	Sample cosine and sine and store as signed 16 bit integers
		int prefactor = 1 << (adctype<bitDepth>::bitDepth - 1);
		for (int k = 0 ; k < N ; k ++)
		{
			coef[k].real() = (typename adctype<bitDepth>::SIGNED)((double)prefactor * std::cos(2*M_PI*(double)k/N));
			coef[k].imag() = (typename adctype<bitDepth>::SIGNED)((double)prefactor * std::sin(2*M_PI*(double)k/N));
		}
		purge();
	}
	
	void purge()
	{
		bzero(data, sizeof(typename adctype<bitDepth>::SIGNED) * N * nchannels);
		bzero(fourier, sizeof(std::complex<typename adctype<bitDepth>::QUADRUPLE_PRECISION::SIGNED>) * N);
		idx = 0;
	}
	
	void update(const typename adctype<bitDepth>::SIGNED *sample)
	{
		//	Slide through circular buffers
		++idx;
		if (idx == N)
			idx = 0;
		
		for (int channel = 0 ; channel < nchannels ; channel ++)
		{
			typename adctype<bitDepth>::DOUBLE_PRECISION::SIGNED diff = sample[channel] - data[idx][channel];
			data[idx][channel] = sample[channel];
			for (int k = 0 ; k < N ; k ++)
			{
				//	Make some convenient shorthands for numbers we need
				typename adctype<bitDepth>::SIGNED coefRe = coef[k].real();
				typename adctype<bitDepth>::SIGNED coefIm = coef[k].imag();
				typename adctype<bitDepth>::QUADRUPLE_PRECISION::SIGNED &fourRe = fourier[k][channel].real();
				typename adctype<bitDepth>::QUADRUPLE_PRECISION::SIGNED &fourIm = fourier[k][channel].imag();
				
				typename adctype<bitDepth>::QUADRUPLE_PRECISION::SIGNED rhsRe = fourRe + diff;
				
				fourRe = (coefRe * rhsRe - coefIm * fourIm) >> 15;
				fourIm = (coefRe * fourIm + coefIm * rhsRe) >> 15;
			}
		}
	}
	
	const std::complex<typename adctype<bitDepth>::QUADRUPLE_PRECISION::SIGNED> &getAmplitude(int k, int channel) const
	{ return fourier[k][channel]; }
};


} // namespace sonar
} // namespace ram


#endif

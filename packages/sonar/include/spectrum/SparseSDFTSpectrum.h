/**
 * @file SparseSDFTSpectrum.h
 * Sliding DFT based spectrum analyzer, inspired by
 * http://www.comm.toronto.edu/~dimitris/ece431/slidingdft.pdf
 * Allows sparse inspection of the frequency range, to avoid computing the whole
 * spectrogram.
 *
 * @author Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 * 
 */

#ifndef _RAM_SPARSESDFTSPECTRUM_H
#define _RAM_SPARSESDFTSPECTRUM_H


#include "Spectrum.h"
#include <cmath>
#include <string.h>

namespace ram {
namespace sonar {

template<int bitDepth, int N, int nchannels, int nFreqBands>
class SparseSDFTSpectrum : Spectrum<bitDepth> {
private:
	int idx;
	typename adctype<bitDepth>::SIGNED data[N][nchannels];
	std::complex<typename adctype<bitDepth>::QUADRUPLE_PRECISION::SIGNED> fourier[nFreqBands][nchannels];
	std::complex<typename adctype<bitDepth>::QUADRUPLE_PRECISION::SIGNED> coef[nFreqBands];
	int kBands[nFreqBands];
public:
	SparseSDFTSpectrum(const int *kBands)
	{
		memcpy(this->kBands, kBands, sizeof(int) * nFreqBands);
		//	Sample cosine and sine and store as signed 16 bit integers
		int prefactor = 1 << (adctype<bitDepth>::bitDepth - 1);
		for (int kIdx = 0 ; kIdx < nFreqBands ; kIdx ++)
		{
			int k = kBands[kIdx];
			coef[kIdx].real() = (typename adctype<bitDepth>::SIGNED)((double)prefactor * std::cos(2*M_PI*(double)k/N));
			coef[kIdx].imag() = (typename adctype<bitDepth>::SIGNED)((double)prefactor * std::sin(2*M_PI*(double)k/N));
		}
		purge();
	}
	
	void purge()
	{
		bzero(data, sizeof(typename adctype<bitDepth>::SIGNED) * N * nchannels);
		bzero(fourier, sizeof(std::complex<typename adctype<bitDepth>::QUADRUPLE_PRECISION::SIGNED>) * nFreqBands);
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
			for (int kIdx = 0 ; kIdx < nFreqBands ; kIdx ++)
			{
				//	Make some convenient shorthands for numbers we need
				typename adctype<bitDepth>::SIGNED coefRe = coef[kIdx].real();
				typename adctype<bitDepth>::SIGNED coefIm = coef[kIdx].imag();
				typename adctype<bitDepth>::QUADRUPLE_PRECISION::SIGNED &fourRe = fourier[kIdx][channel].real();
				typename adctype<bitDepth>::QUADRUPLE_PRECISION::SIGNED &fourIm = fourier[kIdx][channel].imag();
				
				typename adctype<bitDepth>::QUADRUPLE_PRECISION::SIGNED rhsRe = fourRe + diff;
				
				fourRe = (coefRe * rhsRe - coefIm * fourIm) >> (adctype<bitDepth>::bitDepth - 1);
				fourIm = (coefRe * fourIm + coefIm * rhsRe) >> (adctype<bitDepth>::bitDepth - 1);
			}
		}
	}
	
	const typename std::complex<typename adctype<bitDepth>::QUADRUPLE_PRECISION::SIGNED> &getAmplitudeForBinIndex(int kIdx, int channel) const
	{ return fourier[kIdx][channel]; }
	
	int getBinIndexForBin(int k) const
	{
		for (int kIdx = 0 ; kIdx < nFreqBands ; kIdx ++)
			if (kBands[kIdx] == k)
				return kIdx;
		return -1;
	}
	
	const std::complex<typename adctype<bitDepth>::QUADRUPLE_PRECISION::SIGNED> &getAmplitude(int k, int channel) const
	{ return fourier[getBinIndexForBin(k)][channel]; }
};


} // namespace sonar
} // namespace ram


#endif

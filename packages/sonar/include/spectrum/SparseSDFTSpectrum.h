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

template<typename ADC, int N, int nchannels, int nFreqBands>
class SparseSDFTSpectrum : Spectrum<ADC> {
private:
	int idx;
	typename ADC::SIGNED data[N][nchannels];
	std::complex<typename ADC::DOUBLE_WIDE::SIGNED> fourier[nFreqBands][nchannels];
	std::complex<typename ADC::SIGNED> coef[nFreqBands];
	int kBands[nFreqBands];
public:
	SparseSDFTSpectrum(const int *kBands)
	{
		memcpy(this->kBands, kBands, sizeof(int) * nFreqBands);
		//	Sample cosine and sine and store as signed 16 bit integers
		for (int kIdx = 0 ; kIdx < nFreqBands ; kIdx ++)
		{
			int k = kBands[kIdx];
			coef[kIdx].real() = (typename ADC::SIGNED)
				((double)ADC::SIGNED_MAX * std::cos(2*M_PI*(double)k/N));
			coef[kIdx].imag() = (typename ADC::SIGNED)
				((double)ADC::SIGNED_MAX * std::sin(2*M_PI*(double)k/N));
		}
		purge();
	}
	
	void purge()
	{
		bzero(data, sizeof(**data) * N * nchannels);
		bzero(fourier, sizeof(**fourier) * nFreqBands * nchannels);
		idx = N - 1;
	}
	
	void update(const typename ADC::SIGNED *sample)
	{
		memcpy(data[idx], sample, sizeof(*sample) * nchannels);
		//	Slide through circular buffers
		++idx;
		if (idx == N)
			idx = 0;
		
		for (int channel = 0 ; channel < nchannels ; channel ++)
		{
			typename ADC::DOUBLE_WIDE::SIGNED diff = sample[channel] - data[idx][channel];
			for (int kIdx = 0 ; kIdx < nFreqBands ; kIdx ++)
			{
				//	Make some convenient shorthands for numbers we need
				typename ADC::SIGNED coefRe = coef[kIdx].real();
				typename ADC::SIGNED coefIm = coef[kIdx].imag();
				typename ADC::DOUBLE_WIDE::SIGNED &fourRe = fourier[kIdx][channel].real();
				typename ADC::DOUBLE_WIDE::SIGNED &fourIm = fourier[kIdx][channel].imag();
				
				typename ADC::DOUBLE_WIDE::SIGNED rhsRe = fourRe + diff;
				
				fourRe = (typename ADC::DOUBLE_WIDE::SIGNED)(((typename ADC::QUADRUPLE_WIDE::SIGNED)coefRe * rhsRe - (typename ADC::QUADRUPLE_WIDE::SIGNED)coefIm * fourIm) >> (ADC::BITDEPTH - 1));
				fourIm = (typename ADC::DOUBLE_WIDE::SIGNED)(((typename ADC::QUADRUPLE_WIDE::SIGNED)coefRe * fourIm + (typename ADC::QUADRUPLE_WIDE::SIGNED)coefIm * rhsRe) >> (ADC::BITDEPTH - 1));
			}
		}
	}
	
	const typename std::complex<typename ADC::DOUBLE_WIDE::SIGNED> &getAmplitudeForBinIndex(int kIdx, int channel) const
	{ return fourier[kIdx][channel]; }
	
	int getBinIndexForBin(int k) const
	{
		for (int kIdx = 0 ; kIdx < nFreqBands ; kIdx ++)
			if (kBands[kIdx] == k)
				return kIdx;
		return -1;
	}
	
	const std::complex<typename ADC::DOUBLE_WIDE::SIGNED> &getAmplitude(int k, int channel) const
	{ return fourier[getBinIndexForBin(k)][channel]; }
};


} // namespace sonar
} // namespace ram


#endif

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

template<int N, int nchannels, int nFreqBands>
class SparseSDFTSpectrum {
private:
	int idx;
	int16_t data[N][nchannels];
	std::complex<int64_t> fourier[nFreqBands][nchannels];
	std::complex<int32_t> coef[nFreqBands];
	int kBands[nFreqBands];
public:
	SparseSDFTSpectrum(const int *kBands)
	{
		memcpy(this->kBands, kBands, sizeof(int) * nFreqBands);
		//	Sample cosine and sine and store as signed 16 bit integers
		int prefactor = 1 << 15;
		for (int kIdx = 0 ; kIdx < nFreqBands ; kIdx ++)
		{
			int k = kBands[kIdx];
			coef[kIdx].real() = prefactor * std::cos(2*M_PI*(double)k/N);
			coef[kIdx].imag() = prefactor * std::sin(2*M_PI*(double)k/N);
		}
		purge();
	}
	
	void purge()
	{
		bzero(data, sizeof(int16_t) * N * nchannels);
		bzero(fourier, sizeof(std::complex<int64_t>) * nFreqBands);
		idx = 0;
	}
	
	void update(const int16_t *sample)
	{
		//	Slide through circular buffers
		++idx;
		if (idx == N)
			idx = 0;
		
		for (int channel = 0 ; channel < nchannels ; channel ++)
		{
			int32_t diff = sample[channel] - data[idx][channel];
			data[idx][channel] = sample[channel];
			for (int kIdx = 0 ; kIdx < nFreqBands ; kIdx ++)
			{
				//	Make some convenient shorthands for numbers we need
				int32_t coefRe = coef[kIdx].real();
				int32_t coefIm = coef[kIdx].imag();
				int64_t &fourRe = fourier[kIdx][channel].real();
				int64_t &fourIm = fourier[kIdx][channel].imag();
				
				int64_t rhsRe = fourRe + diff;
				
				fourRe = (coefRe * rhsRe - coefIm * fourIm) >> 15;
				fourIm = (coefRe * fourIm + coefIm * rhsRe) >> 15;
			}
		}
	}
	
	const std::complex<int64_t> &getAmplitudeForBinIndex(int kIdx, int channel) const
	{ return fourier[kIdx][channel]; }
	
	int getBinIndexForBin(int k) const
	{
		for (int kIdx = 0 ; kIdx < nFreqBands ; kIdx ++)
			if (kBands[kIdx] == k)
				return kIdx;
		return -1;
	}
	
	const std::complex<int64_t> &getAmplitude(int k, int channel) const
	{ return fourier[getBinIndexForBin(k)][channel]; }
};


} // namespace sonar
} // namespace ram


#endif

/**
 * TestSparseSDFTSpectrum.cpp
 *
 * @author Leo Singer
 * @author Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 */

#include <UnitTest++/UnitTest++.h>
#include <cstdlib>

#include "Sonar.h"
#include "spectrum/SDFTSpectrum.h"
#include "spectrum/SparseSDFTSpectrum.h"

using namespace ram::sonar;
using namespace std;

TEST(SparseSDFTSpectrumGivesSameResultsAsDense)
{
	static const int nSamples = 4096;				//	Number of samples
	static const int nChannels = 4;					//	Number of input channels
	static const int N = 512;						//	Fourier window size
	static const int nKBands = 5;					//	Number of k-values to examine
	static const int kBands[5] = {0, 20, 9, 501, 3};//	k-values we want to examine
	typedef adc<16> myadc;
	
	SDFTSpectrum<myadc, N, nChannels> spectrum;
	SparseSDFTSpectrum<myadc, N, nChannels, nKBands> sparseSpectrum(kBands);
	
	for (int i = 0 ; i < nSamples ; i ++)
	{
		adcdata_t sample[nChannels];
		for (int channel = 0 ; channel < nChannels ; channel ++)
			sample[channel] = (adcdata_t)(((double)rand() / RAND_MAX) * myadc::SIGNED_MAX);
		
		spectrum.update(sample);
		sparseSpectrum.update(sample);
		
		for (int channel = 0 ; channel < nChannels ; channel ++)
			for (int kIdx = 0 ; kIdx < nKBands ; kIdx++)
			{
				int k = kBands[kIdx];
				const std::complex<myadc::DOUBLE_WIDE::SIGNED>& denseResult = spectrum.getAmplitude(k, channel);
				const std::complex<myadc::DOUBLE_WIDE::SIGNED>& sparseResult = sparseSpectrum.getAmplitude(k, channel);
				
				CHECK_EQUAL(denseResult, sparseResult);
			}
	}
}

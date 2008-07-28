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
#include "spectrum/FFTWSpectrum.h"
#include "spectrum/SDFTSpectrum.h"

using namespace ram::sonar;
using namespace std;

TEST(FFTWSpectrumGivesSameResultsAsSDFT)
{
	static const int nSamples = 100;			//	Number of samples
	static const int nChannels = 1;				//	Number of input channels
	static const int N = 512;					//	Fourier window size
	typedef adc<16> myadc;
	
	FFTWSpectrum<myadc, N, nChannels> refSpectrum;
	SDFTSpectrum<myadc, N, nChannels> sbjSpectrum;
	
	for (int i = 0 ; i < nSamples ; i ++)
	{
		adcdata_t sample[nChannels];
		for (int channel = 0 ; channel < nChannels ; channel ++)
			sample[channel] = (adcdata_t)(((double)rand() / RAND_MAX) * (1 << 15));
		
		refSpectrum.update(sample);
		sbjSpectrum.update(sample);
		
		for (int channel = 0 ; channel < nChannels ; channel ++)
        {
			for (int k = 0 ; k < N ; k++)
			{
				const std::complex<myadc::DOUBLE_WIDE::SIGNED>& refResult = refSpectrum.getAmplitude(k, channel);
				const std::complex<myadc::DOUBLE_WIDE::SIGNED>& sbjResult = sbjSpectrum.getAmplitude(k, channel);
				//	Answer diverges from FFTW as the spectrum analzyer becomes older.
				//	Need to quantify rate of divergence.
				CHECK_CLOSE(refResult.real(), sbjResult.real(), 4096);
				CHECK_CLOSE(refResult.imag(), sbjResult.imag(), 4096);
			}
        }
	}
}

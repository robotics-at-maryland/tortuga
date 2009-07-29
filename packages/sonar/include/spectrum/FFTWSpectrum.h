/**
 * @file FFTWSpectrum.h
 * FFTW3-driven spectrum analyzer, provided for verification purposes
 *
 * @author Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 * 
 */

#ifndef _RAM_FFTWSPECTRUM_H
#define _RAM_FFTWSPECTRUM_H

#include "Spectrum.h"
#include <string.h>
#include <fftw3.h>
#include <iostream>

namespace ram {
namespace sonar {

template<typename ADC, int N, int nchannels>
class FFTWSpectrum : Spectrum<ADC> {
private:
	
	/**
	 * The index into the circular data buffer
	 */
	int idx;
	
	/** 
	 * Time-domain input data
	 */
	typename ADC::SIGNED data[N][nchannels];
	
	/**
	 * Array for storing the time-domain input data after it has been cast
	 * to doubles for feeding to FFTW3
	 */
	double dataDouble[N][nchannels];	//	Array for casting data to doubles
	
	/**
	 * Array of complex fixed-point numbers for storing the output in its final
	 * fixed point form
	 */
	std::complex<typename ADC::DOUBLE_WIDE::SIGNED> fourier[N][nchannels];
	
	/**
	 * Array of complex doubles for storing output from FFTW3
	 */
	std::complex<double> fourierDouble[N][nchannels];
	
	/**
	 * Planner structure
	 */
	fftw_plan plan;
public:
	FFTWSpectrum()
	{
		double* in_re = *dataDouble;
		fftw_complex* out_c = reinterpret_cast<fftw_complex*> (*fourierDouble);
		const int dim = N;
		
		purge();
		
		plan = fftw_plan_many_dft_r2c(
			1,			//	rank (1-dimensional)
			&dim,		//	n (size of fourier window)
			nchannels,	//	howmany (number of data series a.k.a. channels)
			in_re,		//	in (pointer to time domain input)
			NULL,		//	inembed
			nchannels,	//	istride
			1,			//	idist
			out_c,		//	out
			NULL,		//	onembed
			nchannels,	//	ostride
			1,			//	odist
			FFTW_PATIENT);
	}
	
	~FFTWSpectrum()
	{
		fftw_destroy_plan(plan);
	}
	
	void purge()
	{
		bzero(data, sizeof(**data) * N * nchannels);
		bzero(fourier, sizeof(**fourier) * N * nchannels);
		idx = N - 1;
	}
	
	void update(const typename ADC::SIGNED* sample)
	{
		//	Pump the new data into the array.
		memcpy(data[idx], sample, sizeof(*sample) * nchannels);
		
		//	Increment the index into the circular buffer.
		++idx;
		if (idx == N)
			idx = 0;
		
		//	Convert the data to floating point format and flatten out the
		//	circular buffer.
		for (int channel = 0 ; channel < nchannels ; channel ++)
		{
			for (int i = idx ; i < N ; i ++)
				dataDouble[i - idx][channel] = (double)(data[i][channel]);
			for (int i = 0 ; i < idx ; i ++)
				dataDouble[i + N - idx][channel] = (double)(data[i][channel]);
		}
		
		//	Run FFTW.
		fftw_execute(plan);
		
		//	Copy the results to fixed-point format.
		for (int channel = 0 ; channel < nchannels ; channel ++)
		{
			//	FFTW thinks it's smart because it takes care of aliasing.
			for (int k = 0 ; 2*k < N ; k ++)
			{
				fourier[k][channel].real() = fourierDouble[k][channel].real();
				fourier[k][channel].imag() = fourierDouble[k][channel].imag();
			}
			for (int k = N / 2 ; k < N ; k ++)
			{
				fourier[k][channel].real() = fourierDouble[N - k][channel].real();
				fourier[k][channel].imag() = -fourierDouble[N - k][channel].imag();
			}
		}
		
	}
	
	const std::complex<typename ADC::DOUBLE_WIDE::SIGNED> &getAmplitude(int k, int channel) const
	{
		return fourier[k][channel];
	}
};


} // namespace sonar
} // namespace ram


#endif

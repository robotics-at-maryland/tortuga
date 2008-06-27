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

namespace ram {
namespace sonar {

template<int N, int nchannels>
class FFTWSpectrum {
private:
	int idx;
	int16_t data[N][nchannels];
	double *in_re;			//	Pointer to input array
	fftw_complex *out_c;	//	Pointer to output array
	fftw_plan plan;			//	FFTW3 planner structure
public:
	FFTWSpectrum()
	{
		in_re = (double*) fftw_malloc(sizeof(double) * N * nchannels);
		out_c = (fftw_complex*) fftw_malloc(sizeof(double) * N * 2 * nchannels);
		int twiceN = 2 * N;
		plan = fftw_plan_many_dft_r2c(
			1,			//	rank
			&N,			//	n
			nChannels,	//	howmany
			in_re,		//	in
			&N,			//	inembed
			nChannels,	//	istride
			1,			//	idist
			out_c,		//	out
			&twiceN,	//	onembed
			nChannels,	//	ostride
			1,			//	odist
			FFTW_PATIENT);
		purge();
	}
	
	~FFTW3Spectrum()
	{
		fftw_destroy_plan(plan);
		fftw_free(in_re);
		fftw_free(out_c);
	}
	
	void purge()
	{
		bzero(data, sizeof(int16_t) * N * nchannels);
		idx = 0;
	}
	
	void update(const int16_t *sample)
	{
		++idx;
		if (idx == N)
			idx = 0;
		
		for (int i = idx ; i < N ; i ++)
			for (int channel = 0 ; channel < nchannels ; channel ++)
				in_re[N * channel + (i-idx)] = data[i][channel];
		for (int i = 0 ; i < idx ; i ++)
			for (int channel = 0 ; channel < nchannels ; channel ++)
				in_re[N * channel + (N-idx+i)] = data[i][channel];
		
		fftw_execute(plan);
	}
	
	const std::complex<int64_t> &getAmplitude(int k, int channel) const
	{
		int64_t re = out_c[2*(channel * N + k)];
		int64_t im = out_c[2*(channel * N + k)+1];
		std::complex<int64_t> result(re, im);
		return result;
	}
};


} // namespace sonar
} // namespace ram


#endif

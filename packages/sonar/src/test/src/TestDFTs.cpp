#include <UnitTest++/UnitTest++.h>
#include <fftw3.h>
#include <stdlib.h>
#include <math.h>
#include "Sonar.h"
#include "dft/SlidingDFT.h"

using namespace ram::sonar;

/**
 * Return a random double between -1 and 1.
 */
double rand_real()
{
	return 2 * (0.5 - ((double) rand()) / RAND_MAX);
}


void adcdata_vector_to_real(const adcdata_t *from, double *to, int len)
{
	for (int i = 0 ; i < len ; i ++)
		to[i] = (double) from[i];
}


/**
 * Return a vector with the specified length, filled with random adcdata_t's
 */
void rand_adcdata_vector(adcdata_t *vec, int length, int seed)
{
	srand(seed);
	for (int i = 0 ; i < length ; i ++)
		vec[i] = (adcdata_t) (rand_real() * adc<10>::SIGNED_MAX());
}


void cos_adcdata_vector(adcdata_t *vec, int nchannels, int k, int N)
{
	for (int n = 0 ; n < N ; n ++)
	{
		adcdata_t cosine = (adcdata_t) 
		(cos(2 * M_PI * n * k / N) * adc<10>::SIGNED_MAX());
		
		for (int channel = 0 ; channel < nchannels ; channel ++)
			vec[n*nchannels + channel] = cosine;
	}
}


adcmath_t normalize_double(double y)
{
	return (adcmath_t) round(y * adc<10>::SIGNED_MAX());
}


TEST_UTILITY(CheckAgainstFFTW, (const adcdata_t *adcdataSamples, int countFrames, SlidingDFT<adc<10> > &myDFT, bool shiftPhase))
{
	int nchannels = myDFT.getCountChannels();
	int k = myDFT.getFourierIndex();
	int N = myDFT.getWindowSize();
	int countInputData = nchannels * countFrames;
	
	//  A parameter needed by fftw
	int *n = new int[nchannels];
	for (int i = 0 ; i < nchannels ; i ++)
		n[i] = N;
	
	double *doubleSamples = new double[countInputData];
	adcdata_vector_to_real(adcdataSamples, doubleSamples, countInputData);
	
	//  Allocate a destination array for fftw.
	fftw_complex *out = 
	(fftw_complex*) fftw_malloc(N * nchannels * sizeof(fftw_complex));
	
	//  The sliding DFT will not give useful results until its whole buffer
	//  (of length N) is full.
	for (int i = 0 ; i < N ; i ++)
		myDFT.update(&adcdataSamples[i * nchannels]);
	
    for (int i = N ; i < countFrames ; i ++)
    {
    	//  Set up the DFT with fftw.  We are using fftw as a trusted 
    	//  reference DFT.  TiledSlidingDFT should give equivalent results.
		fftw_plan p = fftw_plan_many_dft_r2c(1, n, nchannels,
											 &doubleSamples[(i - N) * nchannels], NULL,
											 nchannels, 1,
											 out, NULL,
											 nchannels, 1,
											 FFTW_ESTIMATE);
		
		//  Execute the DFT.
		fftw_execute(p);
		
		//  The sliding DFT picks up a running phase shift as compared to the 
		//  trusted DFT with a moving window.  re_shift and im_shift represent 
		//  the real and imaginary components of this shift.
		double re_shift = cos(-2 * M_PI * (i) * k / N);
		double im_shift = sin(-2 * M_PI * (i) * k / N);
		
		for (int channel = 0 ; channel < nchannels ; channel ++)
		{
			//  The real and imaginary parts of the trusted DFT.
			double re_trusted = out[nchannels * k + channel][0];
			double im_trusted = out[nchannels * k + channel][1];
			
			double re_propagated, im_propagated;
			if (shiftPhase)
			{
				//  The real and imaginary components of the trusted DFT, with the 
				//  propagating phase shift taken into account.
				re_propagated = re_trusted * re_shift - im_trusted * im_shift;
				im_propagated = re_trusted * im_shift + im_trusted * re_shift;
			}
			else
			{
				re_propagated = re_trusted;
				im_propagated = im_trusted;
			}
			
			//  Convert fftw's output to the same normalization as our sliding
			//  DFT.
			adcmath_t re_theirs = normalize_double(re_propagated);
			adcmath_t im_theirs = normalize_double(im_propagated);
			//adcmath_t L1_theirs = normalize_double(abs(re_propagated) + abs(im_propagated));
			
			//  Get results from our sliding DFT.
			adcmath_t re_mine = myDFT.getReal(channel);
			adcmath_t im_mine = myDFT.getImag(channel);
			//adcmath_t L1_mine = myDFT.getMagL1(channel);
			
			//  Compare them.
			CHECK_EQUAL(re_mine, re_theirs);
			CHECK_EQUAL(im_mine, im_theirs);
			//	CHECK_EQUAL(L1_mine, L1_theirs);
		}
		
		//  Update the sliding DFT.
		myDFT.update(&adcdataSamples[i * nchannels]);
		
		//  Throw out the old fftw plan to prepare for a new one.
		fftw_destroy_plan(p);
	}
	
    fftw_free(out);
	delete [] doubleSamples;
	delete [] n;
}

#include "TestTiledSlidingDFT.inl"
//	#include "TestFastSlidingDFT.inl"

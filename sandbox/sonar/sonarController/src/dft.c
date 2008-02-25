/*
 *  dft.c
 *  sonarController
 *
 *  Created by Leo Singer on 2/25/08.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */


#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


#include "SonarCFriendly.h"


typedef struct {
	int k, N, nchannels;
	int idx;
	adcdata_t *bufre, *bufim;
	adcdata_t *coefre, *coefim;
	adcmath_t *re, *im, *mag;
} sliding_dft_t;


sliding_dft_t *dft_malloc(int nchannels, int k, int N);
void dft_free(sliding_dft_t *dft);
void dft_update(sliding_dft_t *dft, adcdata_t *sample);
void dft_setup_coefficients(sliding_dft_t *dft);
void dft_setup_window(sliding_dft_t *dft);
void dft_purge(sliding_dft_t *dft);


sliding_dft_t *dft_malloc(int nchannels, int k, int N)
{
	sliding_dft_t *dft = malloc(sizeof(sliding_dft_t));
	dft_setup_coefficients(dft);
	dft_setup_window(dft);
	return dft;
}


void dft_free(sliding_dft_t *dft)
{
	free(dft->bufre);
	free(dft->bufim);
	free(dft->coefre);
	free(dft->coefim);
	free(dft->re);
	free(dft->im);
	free(dft->mag);
	free(dft);
}


/** Update the one selected Fourier amplitude using a sliding DFT
 */
void dft_update(sliding_dft_t *dft, adcdata_t *sample)
{
	int channel;
	for (channel = 0 ; channel < dft->nchannels ; channel ++)
	{
		/*	We subtract window____[channel][firstidx] because both windowreal[i]
		 *	and windowimag[i] are circular buffers.  On the next call of this 
		 *	function, window____[channel][firstidx] will be overwritten with 
		 *	computations from the next sample.
		 */
		
		dft->re[channel] -= dft->bufre[dft->idx * dft->nchannels + channel];
		dft->im[channel] -= dft->bufim[dft->idx * dft->nchannels + channel];
		
		/*	For each sample we receive, we only need to compute one new term in
		 *	the DFT sum.  These two lines together compute 
		 *
		 *		f(N-1) x cos(2 pi k (N - 1) / N) 
		 *
		 *	and
		 *
		 *		f(N-1) x sin(2 pi k (N - 1) / N)
		 *
		 *	which are, respectively, the real and imaginary parts of 
		 *	
		 *		f(N-1) x exp(2 pi i k (N - 1) / N)
		 *	
		 *	which is simply the last term of the sum for F(k).
		 */
		
		dft->bufre[dft->idx * dft->nchannels + channel] = (adcmath_t) dft->coefre[dft->idx] * sample[channel];
		dft->bufim[dft->idx * dft->nchannels + channel] = (adcmath_t) dft->coefim[dft->idx] * sample[channel];
		
		/*	The next two lines update the real and imaginary part of the complex
		 *	output amplitude.
		 *	
		 *	We add window____[channel][curidx] to sum____[chan constnel] because all 
		 *	of the previous N-1 terms of F(k), numbered 0,1,...,N-3,N-2, were 
		 *	calculated and summed during previous iterations of this function - 
		 *	hence the name "sliding DFT".
		 */
		
		dft->re[channel] += dft->bufre[dft->idx * dft->nchannels + channel];
		dft->im[channel] += dft->bufim[dft->idx * dft->nchannels + channel];
		
		/*	We compute the L1 norm (|a|+|b|) instead of the L2 norm 
		 *	sqrt(a^2+b^2) in order to aovid integer overflow.  Since we are only
		 *	using the magnitude for thresholding, this is an acceptable 
		 *	approximation.
		 */
		
		dft->mag[channel] = abs(dft->re[channel]) + abs(dft->im[channel]);
	}
	
	/*	curidx represents the index into the circular buffers 
	 *	windowreal[channel] and windowimag[channel] at which the just-received
	 *	sample will be added to the DFT sum.
	 */
	
	++(dft->idx);
	if (dft->idx == dft->N)
		dft->idx = 0;
}


void dft_setup_coefficients(sliding_dft_t *dft)
{
	int n;
	dft->coefre = malloc(dft->N*sizeof(adcdata_t));
	dft->coefim = malloc(dft->N*sizeof(adcdata_t));
	for (n = 0 ; n < dft->N ; n++)
	{
		dft->coefre[n] = (adcdata_t) (cos(- 2 * M_PI * n * dft->k / dft->N) * ADCDATA_MAXAMPLITUDE);
		dft->coefim[n] = (adcdata_t) (sin(- 2 * M_PI * n * dft->k / dft->N) * ADCDATA_MAXAMPLITUDE);
	}
}


void dft_setup_window(sliding_dft_t *dft) {
	dft->bufre = malloc(dft->nchannels*dft->N*sizeof(adcmath_t));
	dft->bufim = malloc(dft->nchannels*dft->N*sizeof(adcmath_t));
	dft->re = malloc(dft->nchannels*sizeof(adcmath_t));
	dft->im = malloc(dft->nchannels*sizeof(adcmath_t));
	dft->mag = malloc(dft->nchannels*sizeof(adcmath_t));
	dft_purge(dft);
}


void dft_purge(sliding_dft_t *dft)
{
	memset(dft->re, 0, sizeof(adcmath_t) * dft->nchannels);
	memset(dft->im, 0, sizeof(adcmath_t) * dft->nchannels);
	memset(dft->mag, 0, sizeof(adcmath_t) * dft->nchannels);
	memset(dft->bufre, 0, sizeof(adcdata_t) * dft->N * dft->nchannels);
	memset(dft->bufim, 0, sizeof(adcdata_t) * dft->N * dft->nchannels);
	dft->idx = 0;
}

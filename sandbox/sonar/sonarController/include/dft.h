/*
 *  dft.h
 *  sonarController
 *
 *  Created by Leo Singer on 02/25/08.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 *  C language implementation of the sliding DFT for PIC compatibility.
 */


#ifndef DFT_H
#define DFT_H


#include "Sonar.h"


#ifdef __cplusplus
namespace ram {
namespace sonar {
extern "C" { /* Play nice with C++. */
#endif


typedef struct {
	int k; //bin number
	int N; //number of samples in window
	int nchannels; //number of concurrent channels 
	int idx;
	adcmath_t *bufre, *bufim;
	adcdata_t *coefre, *coefim;
	adcmath_t *re, *im, *mag;
} sliding_dft_t;


//  C30 doesn't do mallocs well, so don't compile dft_alloc if we are on the 
//  C30 platform.
#ifndef __C30

//  Dynamically allocate and initialize a sliding DFT structure
sliding_dft_t *dft_alloc(int nchannels, int k, int N);


//  Free a sliding DFT structure that has been dynamically allocated
void dft_free(sliding_dft_t *dft);

#endif

//  Initialize a sliding DFT structure that has already been allocated,
//  statically or dynamically
void dft_init(sliding_dft_t *dft, int nchannels, int k, int N);

//  Pump new samples into a sliding DFT structure
void dft_update(sliding_dft_t *dft, adcdata_t *sample);

//  Initialize the DFT coefficients.  Called during dft_init
void dft_setup_coefficients(sliding_dft_t *dft);

//  Initialize the DFT window.  Called during dft_init
void dft_setup_window(sliding_dft_t *dft);

//  Purge the DFT window by pumping the buffer full of zeros.  Called during dft_init
void dft_purge(sliding_dft_t *dft);


#ifdef __cplusplus
} /* Done playing nice with C++. */
} // namespace sonar
} // namespace ram
#endif


#endif

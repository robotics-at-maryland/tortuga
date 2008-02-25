/*
 *  dft.h
 *  sonarController
 *
 *  Created by Leo Singer on 02/25/08.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
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
	int k, N, nchannels;
	int idx;
	adcmath_t *bufre, *bufim;
	adcdata_t *coefre, *coefim;
	adcmath_t *re, *im, *mag;
} sliding_dft_t;


sliding_dft_t *dft_alloc(int nchannels, int k, int N);
void dft_free(sliding_dft_t *dft);
void dft_update(sliding_dft_t *dft, adcdata_t *sample);
void dft_setup_coefficients(sliding_dft_t *dft);
void dft_setup_window(sliding_dft_t *dft);
void dft_purge(sliding_dft_t *dft);


#ifdef __cplusplus
} /* Done playing nice with C++. */
} // namespace sonar
} // namespace ram
#endif


#endif

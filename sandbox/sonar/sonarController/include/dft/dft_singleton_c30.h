/*
 *  dft_singleton_c30.h
 *  sonarController
 *
 *  Created by Leo Singer on 03/04/2008.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *  
 *  Statically allocate a single global sliding_dft_t structure.
 *  Some platforms, like PIC30, don't support malloc. 
 *  
 *  Usage:
 * 
 *    #define M_SLIDING_DFT_k 5          // the Fourier bin index
 *    #define M_SLIDING_DFT_N 100        // size of the Fourier window
 *    #define M_SLIDING_DFT_nchannels 3  // number of channels
 *    #include <dft_singleton_c30.h>
 *    
 *    // the rest of your #includes ...
 *    // your function prototypes
 *    
 *    void main(void)
 *    {
 *        ...  // your other variable declarations
 *        sliding_dft_t *dft;
 *        ...  // some code
 *        dft = M_DFT_INIT();
 *        ...  // now you can use the dft with calls to dft_update(dft) and so on.
 *    }
 * 
 */


#include "dft.h"

#ifdef M_SLIDING_DFT_k
#ifdef M_SLIDING_DFT_N
#ifdef M_SLIDING_DFT_nchannels

//	Statically allocate the necessary arrays
adcmath_t M_SLIDING_DFT_BUFRE[M_SLIDING_DFT_N * M_SLIDING_DFT_nchannels];
adcmath_t M_SLIDING_DFT_BUFIM[M_SLIDING_DFT_N * M_SLIDING_DFT_nchannels];
adcdata_t M_SLIDING_DFT_COEFRE[M_SLIDING_DFT_N];
adcdata_t M_SLIDING_DFT_COEFIM[M_SLIDING_DFT_N];
adcmath_t M_SLIDING_DFT_RE[M_SLIDING_DFT_nchannels];
adcmath_t M_SLIDING_DFT_IM[M_SLIDING_DFT_nchannels];
adcmath_t M_SLIDING_DFT_MAG[M_SLIDING_DFT_nchannels];

sliding_dft_t M_SLIDING_DFT = 
{
		M_SLIDING_DFT_k,
		M_SLIDING_DFT_N,
		M_SLIDING_DFT_nchannels,
		0,
		M_SLIDING_DFT_BUFRE,
		M_SLIDING_DFT_BUFIM,
		M_SLIDING_DFT_COEFRE,
		M_SLIDING_DFT_COEFIM,
		M_SLIDING_DFT_RE,
		M_SLIDING_DFT_IM,
		M_SLIDING_DFT_MAG
};

sliding_dft_t *M_DFT_INIT()
{
	dft_init(&M_SLIDING_DFT, M_SLIDING_DFT_k, M_SLIDING_DFT_N, M_SLIDING_DFT_nchannels);
	return &M_SLIDING_DFT;
}

//  Post script to PIC programmers: Call this in void main():
//  dft_init(dft, M_SLIDING_DFT_k, M_SLIDING_DFT_N, M_SLIDING_DFT_nchannels);


#endif
#endif
#endif

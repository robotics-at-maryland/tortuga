/*
 *  sliding_dft_c30.h
 *  sonarController
 *
 *  Created by Leo Singer on 03/04/2008.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *  
 */


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

sliding_dft_t sliding_dft = 
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

sliding_dft_t *dft = &sliding_dft;


#endif
#endif
#endif

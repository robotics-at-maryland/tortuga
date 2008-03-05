#ifndef DFT_SINGLETON_C30_H_
#define DFT_SINGLETON_C30_H_

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

#endif /*DFT_SINGLETON_C30_H_*/

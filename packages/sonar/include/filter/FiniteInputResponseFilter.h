/**
 * @file FiniteInputResponseFilter.h
 * Fixed point finite input response filter
 *
 * @author Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 * 
 */

#ifndef _RAM_SONAR_FILTER_FINITEINPUTRESPONSEFILTER_H
#define _RAM_SONAR_FILTER_FINITEINPUTRESPONSEFILTER_H

#include <cmath>
#include <string.h>
#include <boost/integer/static_log2.hpp>
#include "adctypes.h"
#include <iostream>

namespace ram {
namespace sonar {
namespace filter {

/**
 * @param ADC The adctype (@see adctypes.h)
 * @param N   The order of the filter (N+1 taps)
 * @param nchannels The number of hydrophone channels
 */
template<typename ADC, int N, int nchannels>
class FiniteInputResponseFilter {
private:
    static const int bufLen = N + 1;
	int idx;
    typedef adc<2 * ADC::BITDEPTH + boost::static_log2<bufLen>::value> ACCUM;
	typename ADC::SIGNED X[bufLen][nchannels];
    typename ADC::SIGNED B[bufLen];
    typename ACCUM::SIGNED Y[nchannels];
public:
	FiniteInputResponseFilter(const typename ADC::SIGNED* coefs)
	{
        memcpy(B, coefs, sizeof(*B) * bufLen);
        purge();
	}
	
	void purge()
	{
		bzero(X, sizeof(**X) * bufLen * nchannels);
        bzero(Y, sizeof(*Y) * nchannels);
		idx = N;
	}
	
	void update(const typename ADC::SIGNED *sample)
	{
		//	Slide through circular buffers
		--idx;
		if (idx < 0)
			idx = N;
        
        //  Overwrite the old samples
        memcpy(X[idx], sample, sizeof(*sample)*nchannels);
        
        // Reset output vector
        bzero(Y, sizeof(*Y) * nchannels);
        
        for (int i = 0 ; i + idx < bufLen ; i ++)
            for (int channel = 0 ; channel < nchannels ; channel ++)
                Y[channel] += (typename ADC::DOUBLE_WIDE::SIGNED)B[i] * X[i + idx][channel];
        for (int i = bufLen - idx ; i < bufLen ; i ++)
            for (int channel = 0 ; channel < nchannels ; channel ++)
                Y[channel] += (typename ADC::DOUBLE_WIDE::SIGNED)B[i] * X[i - bufLen + idx][channel];
	}
    
    const typename ACCUM::SIGNED& operator[] (int channel)
    {
        return Y[channel];
    }
};

} // namespace filter
} // namespace sonar
} // namespace ram


#endif

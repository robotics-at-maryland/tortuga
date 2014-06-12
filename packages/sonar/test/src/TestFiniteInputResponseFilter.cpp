/**
 * TestFiniteInputResponseFilter.cpp
 *
 * @author Leo Singer
 * @author Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 */

#include <UnitTest++/UnitTest++.h>
#include <cstdlib>

#include "filter/FiniteInputResponseFilter.h"
#include "adctypes.h"

using namespace ram::sonar;

SUITE(TestFiniteInputResponseFilter)
{
    
    TEST(ArbitraryInputsZeroedTaps)
    {
        typedef adc<16> myadc;
        static const int NCHANNELS = 4;
        static const int ORDER = 4;
        myadc::SIGNED taps[ORDER+1] = {0, 0, 0, 0, 0};
        filter::FiniteInputResponseFilter<myadc, ORDER, NCHANNELS> fir(taps);
        for (int i = 0 ; i < 100 ; i ++)
        {
            myadc::SIGNED sample[NCHANNELS];
            for (int channel = 0 ; channel < NCHANNELS ; channel ++)
                sample[channel] = (double)std::rand() / RAND_MAX * myadc::SIGNED_MAX;
            fir.update(sample);
            for (int channel = 0 ; channel < NCHANNELS ; channel ++)
                CHECK_EQUAL(0, fir[channel]);
        }
    }
    
    TEST(ArbitraryInputSingleTrivialTap)
    {
        typedef adc<16> myadc;
        static const int NCHANNELS = 4;
        static const int ORDER = 0;
        myadc::SIGNED taps[ORDER+1] = {2};
        filter::FiniteInputResponseFilter<myadc, ORDER, NCHANNELS> fir(taps);
        for (int i = 0 ; i < 100 ; i ++)
        {
            myadc::SIGNED sample[NCHANNELS];
            for (int channel = 0 ; channel < NCHANNELS ; channel ++)
                sample[channel] = (double)std::rand() / RAND_MAX * myadc::SIGNED_MAX;
            fir.update(sample);
            for (int channel = 0 ; channel < NCHANNELS ; channel ++)
                CHECK_EQUAL(taps[0] * sample[channel], fir[channel]);
        }
    }
    
    TEST(ArbitraryInputWithSingleDelay)
    {
        typedef adc<16> myadc;
        static const int NCHANNELS = 4;
        static const int ORDER = 1;
        myadc::SIGNED taps[ORDER+1] = {0, 1};
        filter::FiniteInputResponseFilter<myadc, ORDER, NCHANNELS> fir(taps);
        myadc::SIGNED sample[NCHANNELS];
        myadc::SIGNED oldSample[NCHANNELS];
        bzero(sample, sizeof(*sample) * NCHANNELS);
        bzero(oldSample, sizeof(*sample) * NCHANNELS);
        for (int i = 0 ; i < 100 ; i ++)
        {
            for (int channel = 0 ; channel < NCHANNELS ; channel ++)
                sample[channel] = (double)std::rand() / RAND_MAX * myadc::SIGNED_MAX;
            fir.update(sample);
            for (int channel = 0 ; channel < NCHANNELS ; channel ++)
                CHECK_EQUAL(oldSample[channel], fir[channel]);
            memcpy(oldSample, sample, sizeof(*sample) * NCHANNELS);
        }
    }
    
    TEST(ArbitraryInputWithMultipleDelay)
    {
        typedef adc<16> myadc;
        static const int NCHANNELS = 4;
        static const int ORDER = 10;
        myadc::SIGNED taps[ORDER+1] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
        filter::FiniteInputResponseFilter<myadc, ORDER, NCHANNELS> fir(taps);
        myadc::SIGNED sample[ORDER+1][NCHANNELS];
        bzero(sample, sizeof(**sample) * NCHANNELS * (ORDER+1));
        int idx = 0;
        for (int i = 0 ; i < 100 ; i ++)
        {
            ++idx;
            if (idx >= ORDER+1)
                idx = 0;
            
            for (int channel = 0 ; channel < NCHANNELS ; channel ++)
                sample[idx][channel] = (double)std::rand() / RAND_MAX * myadc::SIGNED_MAX;
            fir.update(sample[idx]);
            
            int oldestIdx = idx + 1;
            if (oldestIdx >= ORDER+1)
                oldestIdx = 0;
            
            for (int channel = 0 ; channel < NCHANNELS ; channel ++)
                CHECK_EQUAL(sample[oldestIdx][channel], fir[channel]);
        }
    }
    
}


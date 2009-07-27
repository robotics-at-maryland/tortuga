/**
 * rtrt.cpp
 * Real Time Recursive Templates
 *
 * @author Leo Singer
 * @author Copyright 2009 Robotics@Maryland. All rights reserved.
 *
 */

#include <cmath>
#include <stdint.h>
#include <boost/math/distributions/normal.hpp>

using namespace std;
using namespace boost::math;

// Normal distributions for our two indicators
static const normal_distribution<float> ssqrNoise(433710.448159573, 116596096.451538);
static const normal_distribution<float> ssqrPing (98222212.9212072, 98222212.9212072);
static const normal_distribution<float> sdftNoise(5.4053849545956 , 4.26103977201094);
static const normal_distribution<float> sdftPing (68.6328162979193, 31.3704617539082);

// Measured DC offsets of each channel
static const float dcOffset[] = {-479.7099, -800.4639, -757.1132, -1934.3431};

// Parameters for sliding DFT
static const unsigned int NCHANNELS = 4;
static const unsigned int logN = 8;
static const unsigned int N = 1 << logN; // window size
static const unsigned int k = 11; // harmonic
static const complex<float> twid = polar<float>(1, -2*M_PI*k/N);

// Buffers
int16_t sampleBuf[N][NCHANNELS];
uint32_t sqrBuf[N][NCHANNELS];
uint64_t ssqrBuf[N][NCHANNELS];
complex<float> sdftComplex[NCHANNELS];
uint64_t ssqr[NCHANNELS];
int16_t trig[N][NCHANNELS];

uint32_t toa[NCHANNELS];
uint32_t holdoff;
static const uint32_t maxHoldoff = 50000;
static const uint16_t maxTDOA = 1000;

template<class T>
T absSquared(const complex<T>& z)
{ return z.real() * z.real() + z.imag() * z.imag(); }

int main(int argc, char* argv[])
{
    unsigned int idx = 0;
    unsigned long long int sampleCount = 0;
    
    int16_t sample[NCHANNELS];
    
    for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
        toa[channel] = maxTDOA;
    
    while (1 == fread(sample, sizeof(sample), 1, stdin))
    {
        int16_t probScaled[NCHANNELS];
        
        for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
        {
            sample[channel] -= (int16_t)dcOffset[channel];
            
            // Get ssqr, delayed by N samples
            float ssqrDelayed = ssqrBuf[idx][channel];
            float sdft;
            
            // Update ssqr
            ssqr[channel] -= sqrBuf[idx][channel];
            sqrBuf[idx][channel] = (int32_t)sample[channel] * sample[channel];
            ssqr[channel] += sqrBuf[idx][channel];
            ssqrBuf[idx][channel] = ssqr[channel];
            
            // Update sdft
            sdftComplex[channel] = twid * ((float)(sample[channel] - sampleBuf[idx][channel]) + sdftComplex[channel]);
            if (ssqr[channel] == 0)
                sdft = 0;
            else
                sdft = absSquared(sdftComplex[channel]) / (ssqr[channel]);
            
            // Update sample buffer
            sampleBuf[idx][channel] = sample[channel];
            
            // Update probability
#if 0
            // Method 1 corrected: cumulative distribution function
            const float prob = (1 - cdf(ssqrNoise, ssqrDelayed)) * cdf(sdftPing, sdft);
            probScaled[channel] = 100 * prob;
#elif 1
            // Method 1: cumulative distribution function
            const float prob = (1 - cdf(ssqrNoise, ssqrDelayed)) * cdf(sdftPing, sdft) / (1 + cdf(ssqrPing, ssqrDelayed) - cdf(sdftNoise, sdft));
            probScaled[channel] = 100 * prob;
#else
            // Method 2: probability density function
            const float prob = pdf(ssqrNoise, ssqrDelayed) * pdf(sdftPing, sdft) / (pdf(ssqrPing, ssqrDelayed) + pdf(sdftNoise, sdft));
            probScaled[channel] = 1000 * prob;
#endif
            trig[idx][channel] = probScaled[channel];
            
            if (holdoff == 0)
            {
                if (toa[channel] == maxTDOA)
                {
                    if (trig[(idx+N/2) % N][channel] >= 100)
                    {
                        bool pingFound = true;
                        for (unsigned int i = -32 ; i <= 32  ; i ++)
                            if (trig[(i+N/2) % N][channel] > trig[(idx+N/2) % N][channel])
                            {
                                pingFound = false;
                                break;
                            }
                        if (pingFound)
                            toa[channel] = 0;
                    }
                } else if (++toa[channel] == maxTDOA) {
                    for (unsigned int ch = 0 ; ch  < NCHANNELS ; ch++)
                        toa[ch] = maxTDOA;
                    holdoff = maxHoldoff;
                }
            }
            
        }
        
        bool pingFound = true;
        for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
            if (toa[channel] == maxTDOA)
            {
                pingFound = false;
                break;
            }
        if (pingFound)
        {
            unsigned int channel;
            for (channel = 0 ; channel < NCHANNELS - 1 ; channel ++)
                printf("%llu,", sampleCount - toa[channel] - 256 - 128);
            printf("%llu\n", sampleCount - toa[channel] - 256 - 128);
            
            for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
                toa[channel] = maxTDOA;
            holdoff = maxHoldoff;
        }
                
        //fwrite(probScaled, sizeof(probScaled), 1, stdout);
        
        if (++idx >= N)
            idx = 0;
        
        ++sampleCount;
        
        if (holdoff > 0)
            --holdoff;
    }
    
    return 0;
}

#include "fft.h"
#include "math/include/MatrixN.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>

#ifdef __BFIN
#include "drivers/bfin_spartan/include/addresses.h"
#include "drivers/bfin_spartan/include/report.h"
#endif

using namespace ram::math;
using namespace std;



int fd;




/// FIT SETTINGS ///////////////////////////////////////////////////////////////

static const float PING_FIT_THRESHOLD=0.1;
static const float VECTOR_QUAL_THRESHOLD=12;



/// MECHANICAL SETTINGS ////////////////////////////////////////////////////////

static const double SPEED_OF_SOUND = 1500; // meters per second
static const double SAMPLE_RATE = 500000; // samples per second
static const double hydroPlanarArray[3][2] =  //m, coordinate of hydrophones with respect to hydrophone 0
{
    {0, 0.41656},       //hydrophone 1  {x, y} units is meters
    {0.519684, 0.00635},//hydrophone 2
    {0.519684, 0.4318}  //hydrophone 3
};
static const MatrixN H(*hydroPlanarArray, 3, 2);
static const MatrixN Hpinv = (H.transpose() * H).inverse() * H.transpose();



/// ELECTRICAL SETTINGS ////////////////////////////////////////////////////////

static const float avgs[] = {-500.5852, -813.6023, -761.6551, -1946.2537};
static const float stds[] = {44.5275893304495, 40.7157403452424, 40.3731686057006, 46.7725699918266};



/// DSP SETTINGS ///////////////////////////////////////////////////////////////

static const unsigned int LOG2_BLOCKSIZE = 8;
static const unsigned int BLOCKSIZE = 1 << LOG2_BLOCKSIZE;
static const unsigned int NCHANNELS = 4;
static const unsigned int NBLOCKS =  16;
static const unsigned int LOOKBACKBUFSIZE = NBLOCKS * BLOCKSIZE;
static const unsigned int POWER_OVERSAMPLEBITS = 12;


static const unsigned int FFT_SIZE = 512;
static const unsigned int FFT_MINHARMONIC = 20;
static const unsigned int FFT_MAXHARMONIC = 42;
static const unsigned int FFT_NUMHARMONICS = FFT_MAXHARMONIC - FFT_MINHARMONIC;

static const uint16_t HOLDOFF_NSAMPLES = (uint16_t)(500000 * 0.2);
static const unsigned int HOLDON_NSAMPLES = 128;


static int16_t lookbackBuf[NCHANNELS][LOOKBACKBUFSIZE];
static uint64_t powerBuf[NCHANNELS][BLOCKSIZE];
static uint64_t power[NCHANNELS];




/// CODE ///////////////////////////////////////////////////////////////////////


void initADCs()
{
#ifdef __BFIN
    // Initialize ADC
    REG(ADDR_ADCONFIG) = 0x8000;
    REG(ADDR_ADPRESCALER) = 0x0000;
    REG(ADDR_ADCONFIG) = 0x4044;
#endif
}


/**
 * Determine if a sample can be read from the ADCs.
 * @return true if at least 1 sample is available to be read,
 *              false if otherwise
 */
static inline bool isSampleAvailable()
{
#ifdef __BFIN
    return REG(ADDR_FIFO_COUNT1A) >= 1;
#else
    return true;
#endif
}


static inline int16_t getNextSample(uint8_t channel)
{
#ifdef __BFIN
    return REG((ADDR_FIFO_OUT1B - ADDR_FIFO_OUT1A) * channel + ADDR_FIFO_OUT1A);
#else
    int16_t sample;
    if (1 == fread(&sample, sizeof(sample), 1, stdin))
        return sample;
    else
        exit(0);
#endif
}


static void studyPing(uint16_t toas[NCHANNELS], uint8_t harmonic, uint64_t& iSample)
{
    // Convert sample index on arrival to time of arrival, 
    // to time delay on arrival (TDOA)
    MatrixN tdoas(NCHANNELS - 1, 1);
    for (unsigned int channel = 1 ; channel < NCHANNELS ; channel ++)
        tdoas[channel - 1][0] = (SPEED_OF_SOUND / SAMPLE_RATE)
        * ((int16_t)toas[0] - (int16_t)toas[channel]);
    
    // Compute direction of pinger
    MatrixN dirSoln = Hpinv * tdoas;
    double direction[3] = {dirSoln[0][0], dirSoln[1][0], 0};
    
    // Compute residual from fit
    MatrixN tdoas_residual = H * dirSoln - tdoas;
    
    // Compute error, the sum of squares of residuals
    float fit_error = 0;
    for (unsigned int channel = 0 ; channel < NCHANNELS-2 ; channel ++)
        fit_error += tdoas_residual[channel][0] * tdoas_residual[channel][0];
    
    // If the error is too great, reject the ping.
    if (fit_error > PING_FIT_THRESHOLD)
    {
        // BAD FIT
        cout << "BAD FIT" << endl;
        return;
        
    }
    
    // Otherwise, check the normalization of the ping direction.
    
    // x^2 + y^2, the cosine of the angle of elevation
    double elevationCosineSquared = direction[0] * direction[0] + direction[1] * direction[1];
    
    if (elevationCosineSquared > 1.0)
    {
        // If the direction vector is non-normalized...
        
        if (elevationCosineSquared > VECTOR_QUAL_THRESHOLD)
        {
            // And if the norm exceeds 1 by too much, then reject the ping.
            cout << "BAD VECTOR" << endl;
            return;
            
        } else {
            // The normality was within our specified tolerance, so take the
            // altitude to be zero.
            direction[2] = 0;
            
        }
            
    } else {
        // Enforce the normalization by setting the altitude.
        direction[2] = sqrt(1 - elevationCosineSquared);
    }
    
#ifdef __BFIN
    reportPing(fd, // fd
               0,  // status (unused)
               direction[0], // vectorX
               direction[1], // vectorY
               direction[2], // vectorZ
               0,  // range (unused),
               iSample / 500, // timestamp
               iSample, // sample number
               harmonic
               );
#endif
    
    printf("PING FOUND at %0.3f seconds: %0.2f kHz\n", iSample / 500000.0f, harmonic * 500.0f/512.0f);
    for (int i = 0 ; i < 3 ; i ++)
        printf("   %f\n", direction[i]);//(int16_t)toas[0] - (int16_t)toas[i+1]);
    printf("   BEARING: %f\n", atan2(direction[1], direction[0]) * 180.0 / M_PI);
    printf("\n");
    
}


int main()
{
    int nPings = 0;
    uint16_t holdoff = 0;
    uint16_t holdon = 0;
    uint16_t iLookback = 0;
    uint64_t iSample = 0;
    ram::sonar::fft_fract16<FFT_SIZE> fft;
    
    uint64_t noisePower[NCHANNELS];
    for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
        noisePower[channel] = (uint64_t)(((uint64_t)1 << POWER_OVERSAMPLEBITS*2) * BLOCKSIZE * stds[channel] * stds[channel]);

#ifdef __BFIN
    fd = openDevice();
#endif
    
    initADCs();
    
    while (true)
    {
        while (!isSampleAvailable())
            ; // Busy wait until an entire block is available
        
        // Sample each ADC and store in lookback buffer
        for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
            lookbackBuf[channel][iLookback] = getNextSample(channel);
        
        // Update power
        uint8_t iPower = iLookback & (BLOCKSIZE - 1);
        for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
        {
            power[channel] -= powerBuf[channel][iPower];
            int32_t diff = ((int32_t)lookbackBuf[channel][iLookback] << POWER_OVERSAMPLEBITS) - (int32_t)(((int32_t)1 << POWER_OVERSAMPLEBITS) * avgs[channel]);
            powerBuf[channel][iPower] = (int64_t)diff * diff;
            power[channel] += powerBuf[channel][iPower];
        }
        
        if (holdoff == 0)
        {
            bool triggered = true;
            for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
                if (power[channel] < 20 * noisePower[channel])
                    triggered = false;
            
            if (triggered)
            {
                if (holdon < HOLDON_NSAMPLES)
                    ++holdon;
                else {
                    uint8_t loudestHarmonic = 0;
                    {
                        uint64_t fftAccum[FFT_NUMHARMONICS];
                        bzero(fftAccum, sizeof(fftAccum));
                        
                        for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
                        {
                            int16_t fftIn[FFT_SIZE];
                            
                            // Copy the last 512 samples of the current channel to fftIn
                            {
                                if (iLookback >= FFT_SIZE - 1)
                                    memcpy(fftIn, &lookbackBuf[channel][iLookback + 1 - FFT_SIZE], sizeof(*fftIn) * FFT_SIZE);
                                else
                                {
                                    // Note: we are being sloppy with copying from the lookback buffer
                                    // to the fftIn buffer because FFTs are invariant under circular 
                                    // permutation of the input -- aside from phase.
                                    memcpy(fftIn, &lookbackBuf[channel][0], sizeof(*fftIn) * (iLookback + 1));
                                    memcpy(&fftIn[iLookback + 1], &lookbackBuf[channel][LOOKBACKBUFSIZE - FFT_SIZE + iLookback + 1], sizeof(*fftIn) * (FFT_SIZE - iLookback - 1));
                                }
                            }
                            
                            // Perform the FFT
                            fft.doDFT(fftIn);
                            
                            // Accumulate the resultant magnitudes into fftAccum
                            for (unsigned int i = 0 ; i < FFT_NUMHARMONICS ; i ++)
                            {
                                const complex_fract16& F = fft.dftOut[FFT_MINHARMONIC + i];
                                fftAccum[i] += (uint64_t)((int32_t)F.re * F.re) + (uint64_t)((int32_t)F.im * F.im);
                            }
                        }
                        
                        // Determine the ``loudest'' harmonic
                        uint64_t loudestHarmonicPower = fftAccum[0];
                        for (unsigned int i = 1 ; i < FFT_NUMHARMONICS ; i ++)
                            if (fftAccum[i] >= loudestHarmonicPower)
                                loudestHarmonicPower = fftAccum[i], loudestHarmonic = i;
                    }
                    
                    uint16_t edgeSamples[NCHANNELS];
                    bool allEdgesFound = true;
                    for (unsigned int channel = 0 ; channel < NCHANNELS ; channel ++)
                    {
                        uint64_t dupPower = power[channel];
                        uint64_t dupPowerBuf[BLOCKSIZE];
                        memcpy(dupPowerBuf, powerBuf[channel], sizeof(dupPowerBuf));
                        
                        uint16_t iScan = iLookback;
                        do
                        {
                            if (iScan == 0)
                                iScan = LOOKBACKBUFSIZE - 1;
                            else
                                --iScan;
                            
                            uint8_t iPower = iScan & (BLOCKSIZE - 1);
                            
                            dupPower -= dupPowerBuf[iPower];
                            int32_t diff = ((int32_t)lookbackBuf[channel][iScan] << POWER_OVERSAMPLEBITS) - (int32_t)(((int32_t)1 << POWER_OVERSAMPLEBITS) * avgs[channel]);
                            dupPowerBuf[iPower] = (int64_t)diff * diff;
                            dupPower += dupPowerBuf[iPower];                    
                        } while (iScan != iLookback && dupPower > (uint64_t)(2 * noisePower[channel]));
                        
                        if (iScan == iLookback)
                            allEdgesFound = false;
                        else if (iScan > iLookback)
                            edgeSamples[channel] = iScan - iLookback;
                        else
                            edgeSamples[channel] = iScan + LOOKBACKBUFSIZE - iLookback;
                        
                    } // for
                    
                    holdoff = HOLDOFF_NSAMPLES;
                    holdon = 0;
                    
                    // HACK HACK HACK
                    if (edgeSamples[0] == edgeSamples[1] && edgeSamples[0] == edgeSamples[2] && edgeSamples[0] == edgeSamples[3])
                        allEdgesFound = false;
                    
                    if (allEdgesFound)
                    {
                        printf("%d ", nPings);
                        studyPing(edgeSamples, loudestHarmonic + FFT_MINHARMONIC, iSample);
                        nPings ++;
                    }
                }
            }
        } else /* holdoff != 0 */ {
            --holdoff;
            holdon = 0;
        }
        
        if (++iLookback >= LOOKBACKBUFSIZE)
            iLookback = 0;
        
        ++iSample;
    } // while (true)

#ifdef __BFIN
    closeDevice(fd);
#endif
    
    return 0;
}

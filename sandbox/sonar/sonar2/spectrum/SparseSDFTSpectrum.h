/**
 * @file SparseSDFTSpectrum.h
 * Sliding DFT based spectrum analyzer, inspired by
 * http://www.comm.toronto.edu/~dimitris/ece431/slidingdft.pdf
 * Allows sparse inspection of the frequency range, to avoid computing the whole
 * spectrogram.
 *
 * @author Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 * Needs to be defined after
 * Sonar.h
 * Spectrum.h
 * 
 */

#ifndef _RAM_SPARSESDFTSPECTRUM_H
#define _RAM_SPARSESDFTSPECTRUM_H

using namespace std;

namespace ram {
namespace sonar {

template<int N, int nchannels, int nFreqBands>
class SparseSDFTSpectrum {
private:
    int idx;
    int16_t data[N][nchannels];
    complex<adcmath_t> fourier[nFreqBands][nchannels];
    complex<int16_t> coef[nFreqBands];
    int kBands[nFreqBands];
    public:
    SparseSDFTSpectrum(const int *kBands);
    ~SparseSDFTSpectrum();
    void purge();
    void update(const int16_t *sample);
    const complex<adcmath_t> &getAmplitudeForBinIndex(int kIdx, int channel) const;
    int getBinIndexForBin(int k) const;
    const adcmath_t getL1AmplitudeForBinIndex(int kIdx, int channel) const;
    const complex<adcmath_t> &getAmplitude(int k, int channel) const;
};

} // namespace sonar
} // namespace ram

#endif

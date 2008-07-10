//A class to detect the pings produced by our hydrophones

#ifndef _RAM_SONAR_PINGD
#define _RAM_SONAR_PINGD

#include "SparseSDFTSpectrum.h"
#include "Sonar.h"
#include <cmath>
#include "fixed.h"
#include <iostream>

using namespace std;

namespace ram {
namespace sonar {

class pingDetect
{
    adcmath_t temp;
    int count; //counts how many samples were received since last update
    int detected; //stores the hydrophones that detected the ping
    adcmath_t currmax[NCHANNELS]; //current maximum value
    adcmath_t minmax[NCHANNELS]; //minima over the frames
    int threshold[NCHANNELS]; //thresholds for detecting the pings
    int numchan; //number of channels actually used
    SparseSDFTSpectrum <DFT_FRAME, NCHANNELS, nKBands>* spectrum;

    public:
    pingDetect(int* hydro_threshold, int nchan, const int* bands)
    {
        spectrum=new SparseSDFTSpectrum<DFT_FRAME, NCHANNELS, nKBands>(bands);
        numchan=nchan;
        count=0;
        for(int k=0; k<nchan; k++)
        {
            threshold[k]=hydro_threshold[k];
            currmax[k] = 0;
            minmax[k] = adcmath_t(1) << 30;
        }
    }
        
    ~pingDetect() {}

    int p_update(adcdata_t *sample, int kBand)
    {
        detected=0;
        spectrum->update(sample);
        for(int k=0; k<numchan; k++)
        {
            temp=spectrum->getL1AmplitudeForBinIndex(kBand,k);
            //update the maximum
            if(temp>currmax[k])
                currmax[k]=temp;
        }

        ++count;
        if(count==PING_DETECT_FRAME)
        {
            count=0;
            for(int k=0; k<numchan; k++)
            {
                if(currmax[k]<minmax[k])
                    minmax[k]=currmax[k];
                else if(currmax[k] > (threshold[k]*minmax[k]))
                    detected += 1 << k;
                currmax[k]=0;
            }
                    //Adds 1 for channel 1, 2 for 2, 4 for 3, 8 for 4
        }

        return detected;
    }

    void reset_minmax()
    {
        for(int k=0; k<numchan; k++)
            minmax[k] = adcmath_t(1) << 30;
    }

    double get_phase(int k, int kBand)
    {
        const complex<adcmath_t> &ch= spectrum->getAmplitudeForBinIndex(k, kBand);
        complex<double> chDouble(ch.real(),ch.imag());

        return arg(chDouble);
    }

}; //pingDetect
}//sonar
}//ram

#endif

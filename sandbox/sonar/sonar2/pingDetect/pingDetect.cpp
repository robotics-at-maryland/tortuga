/* A class to detect the pings produced by our hydrophones
 * It operates by finding the maxima over intervals in the data.
 * From these, the smallest maximum is calculated.  When the ratio
 * of the smallest maximum to the current maximum is trigerred, the
 * ping is said to be found for that particular channel.
 * It then returns a value corresponding to the hydrophones that trigerred.
 */
#include <cmath>
#include "Sonar.h"
#include "fixed.h"
#include "SparseSDFTSpectrum.h"
#include "pingDetect.h"
#include <iostream>

using namespace std;
using namespace ram::sonar;

/* Constructor for pingDetect class.  Initializes the Fourier Transform,
 * counting variables
 */
pingDetect::pingDetect(const int* hydro_threshold, int nchan, const int* bands, int p_detect_frame)
{
    spectrum=new SparseSDFTSpectrum<DFT_FRAME, NCHANNELS, nKBands>(bands);
    numchan=nchan;
    ping_detect_frame=p_detect_frame;

    count=0;
    detected=0;

    for(int k=0; k<nchan; k++)
    {
        threshold[k]=hydro_threshold[k];
        currmax[k] = 0;
        minmax[k] = adcmath_t(1) << 30;
    }
}
        

pingDetect::~pingDetect()
{
    delete spectrum;
}

/* Updates the Fourier Transform with sample then updates the min-max
 * algorithm.
 * It returns a sum of the channel values indicating if ping was found.
 * 1 -for channel 1, 2 -for ch 2, 4-for ch 3, 8- for chan 4.
 * So, the value is 0 if there were no pings found, 15 if all 4 found.
 */
int
pingDetect::p_update(adcdata_t *sample, int kBand)
{
    detected=0;
    spectrum->update(sample);
    for(int k=0; k<numchan; k++)
    {
        temp=fixed::magL1(spectrum->getAmplitudeForBinIndex(kBand,k));
        if(temp>currmax[k])
            currmax[k]=temp; //update the maximum
    }

    ++count;
    if(count==ping_detect_frame) //if at the end of max frame
    {
        count=0;
        for(int k=0; k<numchan; k++)
        {
            if(currmax[k]<minmax[k])
                minmax[k]=currmax[k];
            else if(currmax[k] > (threshold[k]*minmax[k]))
                detected += (1 << k); //Adds 1 for channel 1, 2 for 2, 4 for 3, 8 for 4
            currmax[k]=0; //reset max, so that it works with the update max for loop
        }
    }

    return detected;
}


/* Resets the smallest maxima to high values.
 * Calling this helps the function eliminate high-amplitude noise
 * and recalibrate itself to the new noise level
 * Normally, would be called after a ping was detected
 */
void
pingDetect::reset_minmax()
{
    for(int k=0; k<numchan; k++)
        minmax[k] = adcmath_t(1) << 30;
}

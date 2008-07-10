/* A class to detect the pings produced by our hydrophones
 * !!!Need to be included along with
 * Sonar.h
 * !!!
 */

#ifndef _RAM_SONAR_PINGD
#define _RAM_SONAR_PINGD

#include "SparseSDFTSpectrum.h"

using namespace std;

namespace ram {
namespace sonar {

class pingDetect
{
    int numchan; //number of channels actually used
    int count; //counts how many samples were received since last update
    int detected; //stores the hydrophones that detected the ping
    adcmath_t temp; //temporary value for manipulating numbers
    adcmath_t currmax[NCHANNELS]; //current maximum value
    adcmath_t minmax[NCHANNELS]; //minima over the frames
    int threshold[NCHANNELS]; //thresholds for detecting the pings
    SparseSDFTSpectrum <DFT_FRAME, NCHANNELS, nKBands>* spectrum; //Fourier transform class

    public:
    pingDetect(int* hydro_threshold, int nchan, const int* bands);
    ~pingDetect();
    int p_update(adcdata_t *sample, int kBand);
    void reset_minmax();
    double get_phase(int k, int kBand);
}; //pingDetect

}//sonar
}//ram

#endif

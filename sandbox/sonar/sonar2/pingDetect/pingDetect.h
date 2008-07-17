/* Michael Levashov
 * Robotics@Maryland
 * A class to detect the pings produced by our hydrophones
 * !!!Need to be included along with
 * Sonar.h
 * SparseSDFTSpectrum.h
 * !!!
 */

#ifndef _RAM_SONAR_PINGD
#define _RAM_SONAR_PINGD

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
    int ping_detect_frame; //the width of frames over which the max is calculated
    SparseSDFTSpectrum <DFT_FRAME, NCHANNELS, nKBands>* spectrum; //Fourier transform class

    public:
    pingDetect(const int* hydro_threshold, int nchan, const int* bands, int p_detect_frame);
    ~pingDetect();
    int p_update(adcdata_t *sample, int kBand);
    void reset_minmax();
}; //pingDetect

}//sonar
}//ram

#endif

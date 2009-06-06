/* Michael Levashov
 * Robotics@Maryland
 *
 * This function is responsible for finding the pings in a chunk of data and filling an array with data that contains the pings 
 * It tries to be smart and pick out the chunks appropriate for each array,
 * while ignoring false positives that are far apart
 * data is an NCHANNEL-sized array of pointers to ENV_CALC_FRAME-sized int arrays
 * The pings are extracted from the dataSet and are stored there
 * locations is a pointer to an NCHANNEL-sized array of integers storing the locations of the first points in the data arrays with respect to the dataSet
 */
//include dataset.h before this file

#ifndef _RAM_SONAR_PING_CHUNK
#define _RAM_SONAR_PING_CHUNK

namespace ram {
namespace sonar {

class getPingChunk {
    int detected;
    pingDetect *pdetect;
    adcdata_t sample[NCHANNELS];
    int last_detected;
    int last_ping_index[NCHANNELS];
    int last_value[NCHANNELS];

    public:
    getPingChunk();
    ~getPingChunk();
    int getChunk(adcdata_t** data, int* locations, dataset* dataSet);
};

}//sonar
}//ram
#endif

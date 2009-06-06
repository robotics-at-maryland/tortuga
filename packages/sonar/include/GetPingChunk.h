/**
 * @file packages/sonar/include/GetPingChunk.h
 *
 * @author Michael Levashov
 * @author Copyright (C) 2008 Robotics at Maryland
 * @author Copyright (C) 2008 Michael Levashov
 * @author All rights reserved.
 */


#ifndef _RAM_SONAR_GETPINGCHUNK_H
#define _RAM_SONAR_GETPINGCHUNK_H

#include "PingDetect.h"

// Forward declare
extern "C" {
    struct dataset;
}
 
namespace ram {
namespace sonar {

/**
 * This function is responsible for finding the pings in a chunk of data and filling an array with data that contains the pings 
 * It tries to be smart and pick out the chunks appropriate for each array,
 * while ignoring false positives that are far apart
 * data is an NCHANNEL-sized array of pointers to ENV_CALC_FRAME-sized int arrays
 * The pings are extracted from the dataSet and are stored there
 * locations is a pointer to an NCHANNEL-sized array of integers storing the locations of the first points in the data arrays with respect to the dataSet
 */
class getPingChunk {
    pingDetect pdetect;

    public:
    getPingChunk(const int* kBands);
    int getChunk(adcdata_t** data, int* locations, struct dataset* dataSet);
};

}//sonar
}//ram
#endif

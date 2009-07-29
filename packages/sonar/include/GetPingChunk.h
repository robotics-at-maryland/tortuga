/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Michael Levashov
 * All rights reserved.
 *
 * Author: Michael Levashov
 * File:  packages/sonar/include/GetPingChunk.h
 */


#ifndef _RAM_SONAR_PING_CHUNK
#define _RAM_SONAR_PING_CHUNK

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
    int detected;
    pingDetect pdetect;
    adcdata_t sample[NCHANNELS];
    int last_detected;
    int last_ping_index[NCHANNELS];
    int last_value[NCHANNELS];

    public:
    getPingChunk(const int* kBands);
    ~getPingChunk();
    int getChunk(adcdata_t** data, int* locations, struct dataset* dataSet);
};

}//sonar
}//ram
#endif

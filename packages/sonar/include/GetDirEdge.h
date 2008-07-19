/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Michael Levashov
 * All rights reserved.
 *
 * Author: Michael Levashov
 * File:  packages/sonar/include/GetDirEdge.h
 */

// Project Includes
#include "sonar/include/Sonar.h"

extern "C" {
    struct dataset;
}

namespace ram {

// Forward declare MatrixN
namespace math{
    class MatrixN;
}
    
namespace sonar {

class getPingChunk;
class sonarPing;
    
class getDirEdge {
    getPingChunk *chunk;
    adcdata_t* data[NCHANNELS];
    int locations[NCHANNELS];
    int64_t total[NCHANNELS];
    int64_t abstotal[NCHANNELS];
    adcdata_t average[NCHANNELS];
    adcdata_t absaverage[NCHANNELS];
    int pingpoints[NCHANNELS];
    int ping_found;
    math::MatrixN *tdoas;
    math::MatrixN *temp_calc; //A matrix for temporary calculations
    math::MatrixN *hydro_array; //A matrix for storing the hydro array

    public:
    getDirEdge();
    ~getDirEdge();
    int getEdge(sonarPing* ping, struct dataset *dataSet);
};
} //sonar
} //ram

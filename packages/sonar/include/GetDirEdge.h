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
#include "sonar/include/GetPingChunk.h"

#include "math/include/MatrixN.h"

extern "C" {
    struct dataset;
}

namespace ram {

namespace sonar {

class getDirEdge {
    getPingChunk chunk;
    adcdata_t* data[NCHANNELS];
    int locations[NCHANNELS];
    int64_t total[NCHANNELS];
    int64_t abstotal[NCHANNELS];
    adcdata_t average[NCHANNELS];
    adcdata_t absaverage[NCHANNELS];
    int pingpoints[NCHANNELS];
    int ping_found;
    math::MatrixN tdoas;
    math::MatrixN temp_calc; //A matrix for temporary calculations
    math::MatrixN hydro_array; //A matrix for storing the hydro array
    math::MatrixN ping_matr; //A final calculated matrix multiplying the tdoas
    math::MatrixN tdoa_errors; //A final calculated matrix multiplying the tdoas
    float fit_error;

    void zero_values();
    public:
    getDirEdge();
    ~getDirEdge();
    int getEdge(sonarPing* ping, struct dataset *dataSet);
};
} //sonar
} //ram

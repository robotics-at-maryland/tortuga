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
private:
    getPingChunk chunk;
    adcdata_t* data[NCHANNELS];
    int locations[NCHANNELS];
    int ping_found;
    math::MatrixN tdoas;
    math::MatrixN temp_calc; //A matrix for temporary calculations
    math::MatrixN hydro_array; //A matrix for storing the hydro array
    math::MatrixN ping_matr; //A final calculated matrix multiplying the tdoas
    math::MatrixN tdoa_errors; //A final calculated matrix multiplying the tdoas

    /**
     * Reset this instance, putting it in the same state it is in just after the
     * constructor is called.
     */
    void purge();
public:
    getDirEdge();
    ~getDirEdge();
    
    /**
     * Attempt to detect a ping and refine the time of arrival estimates.
     *
     * @return -1 An edge was not found on all four channels.
     * @return  0 A bad ping that did not satisfy geometric constraints occurred.
     * @return  1 A ping was detected and successfullly localized.
     */
    int getEdge(sonarPing* ping, struct dataset *dataSet);
};
} //sonar
} //ram

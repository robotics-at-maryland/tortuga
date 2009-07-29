/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Michael Levashov
 * All rights reserved.
 *
 * Author: Michael Levashov
 * File:  packages/sonar/src/GetDirEdge.cpp
 */

// STD Includes
#include <cmath>
#include <iostream>

// Project Includes
#include "math/include/MatrixN.h"

#include "sonar/include/fixed/fixed.h"
#include "sonar/include/Sonar.h"
#include "sonar/include/GetPingChunk.h"
#include "sonar/include/SonarPing.h"
#include "sonar/include/GetDirEdge.h"

#include "drivers/bfin_spartan/include/dataset.h"

namespace ram {
namespace sonar {

using namespace ram::math;
using namespace std;

getDirEdge::getDirEdge(const int* kBands)
    : chunk(kBands), tdoas(3,1), temp_calc(3,3), hydro_array(3,2), ping_matr(3,2), tdoa_errors(3,1)
{
    for(int j=0; j<NCHANNELS; j++)
        data[j]=new adcdata_t [ENV_CALC_FRAME];

    for(int i=0; i<3; i++)
        for(int j=0; j<2; j++)
            hydro_array[i][j]=hydroPlanarArray[i][j];

    //do inv(H'*H)*H'
    temp_calc=hydro_array.transpose()*hydro_array;
    temp_calc.invert();
    temp_calc=temp_calc*hydro_array.transpose();
}

getDirEdge::~getDirEdge()
{
    for(int j=0; j<NCHANNELS; j++)
        delete[] data[j];
}

int getDirEdge::getEdge(sonarPing* ping, dataset *dataSet)
{
    if((ping_found=chunk.getChunk(data, locations, dataSet))==0)
        return 0;
    else if(ping_found != 1)
        return -1;
    
    int pingpoints[NCHANNELS];

    // For each channel ...
    for(int channel=0; channel<NCHANNELS; channel++)
    {
        // Find the average signal over the detected range -- the DC offset. 
        adc<16>::DOUBLE_WIDE::SIGNED average = 0;
        for(int j=0; j<ENV_CALC_FRAME; j++)
            average += data[channel][j];
        average /= ENV_CALC_FRAME;

        // Find the average magnitude of the signal, with the DC offset removed.
        adc<16>::DOUBLE_WIDE::SIGNED absaverage = 0;
        for(int j=0; j<ENV_CALC_FRAME; j++)
            absaverage += fixed::abs(data[channel][j] - average);
        absaverage /= ENV_CALC_FRAME;
        
        // Then, crawl through the samples one more time, and find where the 
        // signal minus the DC offset exceeds the average magnitude for the first
        // time.  This should be the rising edge.
        pingpoints[channel] = -1;
        for(int j=0; j<ENV_CALC_FRAME; j++)
        {
            if(data[channel][j] - average > absaverage)
            {
                // Refine the estimate of the time of arrival with the new
                // information.
                pingpoints[channel] = locations[channel]+j;
                break;
            }
        }
        if (pingpoints[channel] == -1)
            return -1;
    }

    // Compute time delays on arrival (TDOAS) from times of arrival
    for(int channel=0; channel<NCHANNELS-1; channel++)
        tdoas[channel][0]=(SPEED_OF_SOUND * double(pingpoints[channel+1]-pingpoints[0]))/SAMPRATE;

    // Compute direction of pinger
    ping_matr=temp_calc*tdoas;

    // Compute variance between expected TDOA vector, giving location solution,
    // and actual TDOA vector.
    tdoa_errors=hydro_array*ping_matr-tdoas;
    float fit_error = 0;
    for(int channel=0; channel<NCHANNELS-1; channel++)
        fit_error+=tdoa_errors[channel][0]*tdoa_errors[channel][0];

    // If the variance is too great, reject the ping.
    if(fit_error > PING_FIT_THRESHOLD)
    {
        cout<<"BAD FIT\n";
        return 0;
    }

    // Fill in the sonar ping, as a vector towards the pinger
    for(int i=0; i<2; i++)
        ping->direction[i]=-ping_matr[i][0];
    
    //force it to be positive, since we know that the pinger is below
    
    // x^2+y^2
    double temp=ping->direction[0]*ping->direction[0]+ping->direction[1]*ping->direction[1];
    if(temp > 1) // If the vector is non-normalized...
    {
        if(temp>VECTOR_QUAL_THRESHOLD) // By a certain amount ...
        {
            // Then reject the ping.
            cout<<"BAD VECTOR\n";
            return 0;
        }
        else
        {
            // The normality was within our specified tolerance, but just barely,
            // so take the altitude to be zero.
            ping->direction[2]=0;
        }
    }
    else
        ping->direction[2]=-std::sqrt(1-temp);

    ping->point_num=pingpoints[0];
    ping->distance=0;

    // Return success
    return 1;
}
} //sonar
} //ram

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

// Project Includes
#include "sonar/include/GetDirEdge.h"
#include "sonar/include/Sonar.h"
#include "sonar/include/GetPingChunk.h"
#include "sonar/include/SonarPing.h"

//#include "spartan.h"
#include "drivers/bfin_spartan/include/dataset.h"

#include "math/include/MatrixN.h"

#include <iostream>

int64_t myAbs(int64_t x)
{
	if (x < 0)
		return -x;
	else
		return x;
}

namespace ram {
namespace sonar {

using namespace ram::math;
using namespace std;

getDirEdge::getDirEdge()
{
    for(int i=0; i<NCHANNELS; i++)
    {
        total[i]=0;
        abstotal[i]=0;
        pingpoints[i]=0;
    }
    chunk=new getPingChunk();
    tdoas=new MatrixN(3,1);
    temp_calc=new MatrixN(3,3); //A matrix for temporary calculations
    hydro_array=new MatrixN(3,2); //A matrix for storing the hydro array
    for(int j=0; j<NCHANNELS; j++)
        data[j]=new adcdata_t [ENV_CALC_FRAME];
}

getDirEdge::~getDirEdge()
{
    delete chunk;
    delete tdoas;
    delete temp_calc;
    delete hydro_array;
    for(int j=0; j<NCHANNELS; j++)
        delete data[j];
}

int getDirEdge::getEdge(sonarPing* ping, dataset *dataSet)
{
    for(int i=0; i<3; i++)
        for(int j=0; j<2; j++)
            (*hydro_array)[i][j]=hydroPlanarArray[i][j];

    if((ping_found=chunk->getChunk(data, locations, dataSet))==0)
        return 0;
    else if(ping_found != 1)
        return -1;

    for(int i=0; i<NCHANNELS; i++)
    {
        for(int j=0; j<ENV_CALC_FRAME; j++)
            total[i]+=data[i][j];

        average[i]=total[i]/ENV_CALC_FRAME;

        for(int j=0; j<ENV_CALC_FRAME; j++)
            abstotal[i]+=myAbs(data[i][j]-average[i]);

        absaverage[i]=abstotal[i]/ENV_CALC_FRAME;

        for(int j=0; j<ENV_CALC_FRAME; j++)
            if(data[i][j]-average[i]>absaverage[i])
            {
            //cout<<"In set pos "<<i<<" "<<j<<endl;
                pingpoints[i]=locations[i]+j;
                break;
            }
        cout<<"Positions "<<i<<" "<<pingpoints[i]<<endl;
    }

    if((pingpoints[0]==0) ||
       (pingpoints[1]==0) ||
       (pingpoints[2]==0) ||
       (pingpoints[3]==0))
        return -1;

    for(int i=0; i<NCHANNELS-1; i++)
        (*tdoas)[i][0]=(SPEED_OF_SOUND* double(pingpoints[i+1]-pingpoints[0]))/SAMPRATE;
    //do inv(H'*H)*H*tdoas
    (*temp_calc)=hydro_array->transpose()*(*hydro_array);
    (*temp_calc).invert();
    (*temp_calc)=((*temp_calc)*hydro_array->transpose())*(*tdoas);

    //Fill in the sonar ping
    for(int i=0; i<2; i++)
        ping->direction[i]=(*temp_calc)[i][0];
    //force it to be positive, since we know that the pinger is below
    ping->direction[2]=std::sqrt(1-ping->direction[0]*ping->direction[0]-ping->direction[1]*ping->direction[1]);

    ping->point_num=pingpoints[0];
    ping->distance=0;

    return 1;
}
} //sonar
} //ram

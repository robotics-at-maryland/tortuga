/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Michael Levashov
 * All rights reserved.
 *
 * Author: Michael Levashov
 * File:  packages/sonar/src/sonard/main.cpp
 */

/* 
 * This is the main program to run the sonar.
 * It starts off by acquiring a large dataset.
 * It then runs a detection algorithm on it, which gives it the direction from the pinger and the point in the data set at which the ping was found (for hydrophone 0)
 * It passes these parameters onto the main computer for use with the AI 
 * The program times itself from the start of the data collection till the algorithm has completed.
 * From the measurements, it can approximate when the next ping comes, so that it needs to take a smaller dataset
 *
 * The program loops infinitely until the blackfin is shut down or it receives a terminate signal (???) from the main computer
 */

// STD includes
#include <unistd.h>
#include <iostream>

// UNIX Includes
#include <sys/time.h>

// Project Includes
//#include "math/include/MatrixN.h"
//#include "math/include/Vector3.h"

#include "sonar/include/PingerTracker.h"
#include "sonar/include/SonarPing.h"
#include "sonar/include/GetDirEdge.h"
#include "sonar/include/Sonar.h"

#include "drivers/bfin_spartan/include/dataset.h"
#include "drivers/bfin_spartan/include/report.h"
#include "drivers/bfin_spartan/include/spartan.h"

/*#include "SparseSDFTSpectrum.h"
#include "pingDetect.h"
#include "getPingChunk.h"*/

using namespace ram::sonar;
using namespace ram::math;
using namespace std;

dataset* getDataset(dataset *dataSet, int length);

int main(int argc, char* argv[])
{
    if (argc != nKBands)
    {
        cerr << nKBands << " input parameters are required." << endl;
        return -1;
    }
    int myKBands[nKBands];
    for (int i = 0 ; i < nKBands ; i ++)
    {
        myKBands[i] = fixed::round<int>(atof(argv[1 + i])*DFT_FRAME/SAMPRATE);
        double freqEquiv = (double)myKBands[i] * SAMPRATE / DFT_FRAME;
        std::cout << "Band " << i << " set to " << myKBands[i]
            << " (" << freqEquiv << " kHz)" << std::endl;
    }
    
    PingerTracker_go(myKBands);
    
    return 0;
}

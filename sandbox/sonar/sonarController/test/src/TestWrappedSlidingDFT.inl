/*
 *  TestTiledSlidingDFT.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 1/16/08.
 *  Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 */

#include "dft/WrappedSlidingDFT.h"

struct WrappedSlidingDFTTestFixture {};


TEST_FIXTURE(WrappedSlidingDFTTestFixture, CompareDFTRandomInput)
{
	WrappedSlidingDFT<3, 5, 20> myDFT;
	int seed = 42, countFrames = 200;
	int countInputData = myDFT.getCountChannels() * countFrames;
	
	adcdata_t *in = new adcdata_t[countInputData];
	
	rand_adcdata_vector(in, countInputData, seed);
	
	TEST_UTILITY_FUNC(CheckAgainstFFTW)(in, countFrames, myDFT, true);
	
	delete [] in;
}


TEST_FIXTURE(WrappedSlidingDFTTestFixture, CompareDFTCosineInput)
{
	WrappedSlidingDFT<3, 5, 20> myDFT;
	int countFrames = 200;
	int countInputData = myDFT.getCountChannels() * countFrames;
	
	adcdata_t *in = new adcdata_t[countInputData];
	
	cos_adcdata_vector(in, myDFT.getCountChannels(), countFrames/8, countFrames);
	
	TEST_UTILITY_FUNC(CheckAgainstFFTW)(in, countFrames, myDFT, true);
	
	delete [] in;
}

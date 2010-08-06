/**
 * @file spectrogram.cpp
 *
 * @author Leo Singer
 * Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "Sonar.h"

using namespace ram::sonar;
using namespace std;

static const int nChannels = 4;

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		fprintf(stderr, "Please supply a starting sample and an ending sample.\n");
		return EXIT_FAILURE;
	}
	
	long startSample = atol(argv[1]);
	long stopSample = atol(argv[2]);
	
	adcdata_t sample[nChannels];
	
	long sampleCount = 0;
	while (fread(sample, sizeof(adcdata_t), nChannels, stdin) == (size_t)nChannels)
	{
		++sampleCount;
		if (sampleCount > startSample && sampleCount <= stopSample)
			fwrite(sample, sizeof(adcdata_t), nChannels, stdout);
	}
	return 0;
}

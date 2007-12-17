/*
 *  crosscorrelation.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 11/28/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 *  Simple test program for tdoa_xcorr(a,b)
 *
 */


#include <stdio.h>


#include "SonarChunk.h"
#include "TDOA.h"


#define NUM_SENSORS 5


int main (int argc, char * const argv[])
{
	FILE *f;
	ram::sonar::adcdata_t sample[NUM_SENSORS];
	int numsamples = 0;
	ram::sonar::SonarChunk a, b;
	if (argc >= 2)
	{
		f = fopen(argv[1], "rb");
		if (f == NULL)
		{
			fprintf(stderr,"Could not open file.\n");
			return -1;
		}
		while (!feof(f) && numsamples < ram::sonar::SonarChunk::capacity)
		{
			fread(sample, sizeof(ram::sonar::adcdata_t), NUM_SENSORS, f);
			a.append(sample[0]);
			b.append(sample[1]);
			numsamples ++;
		}
		fclose(f);
		printf("Read %d samples from %d sensors.\n", numsamples, NUM_SENSORS);
		printf("Index of  maximum cross correlation: %d\n", 
			   ram::sonar::tdoa_xcorr(a,b));
	}
	else
	{
		fprintf(stderr,"No file specified.\n");
		return -1;
	}
    return 0;
}

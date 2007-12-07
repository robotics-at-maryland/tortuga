/*
 *  main.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 11/28/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */


#include <stdio.h>


#include "SonarController.h"


#define NUM_SENSORS 4


int main (int argc, char * const argv[])
{
	FILE *f;
	SonarController sc(NUM_SENSORS);
	adcdata_t sample[NUM_SENSORS];
	int numsamples = 0;
	if (argc >= 2)
	{
		f = fopen(argv[1], "rb");
		if (f == NULL)
		{
			fprintf(stderr,"Could not open file.");
			return -1;
		}
		sc.go();
		while (!feof(f))
		{
			fread(sample, sizeof(adcdata_t), NUM_SENSORS, f);
			sc.receiveSample(sample);
			numsamples ++;
		}
		fclose(f);
		printf("Read %d samples from %d sensors.\n", numsamples, NUM_SENSORS);
	}
	else
	{
		fprintf(stderr,"No file specified.\n");
		return -1;
	}
    return 0;
}

/*
 *  main.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 11/28/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */


#include <stdio.h>
#include <time.h>


#include "SonarController.h"


#define NUM_SENSORS 4

clock_t clock_io(char * fname);

int main (int argc, char * const argv[])
{
	FILE *f;
	clock_t clock_run, clock_input;
	ram::sonar::SonarController sc(NUM_SENSORS);
	ram::sonar::adcdata_t sample[NUM_SENSORS];
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
		clock_run = clock();
		while (!feof(f))
		{
			fread(sample, sizeof(ram::sonar::adcdata_t), NUM_SENSORS, f);
			sc.receiveSample(sample);
			numsamples ++;
		}
		clock_run = clock() - clock_run;
		fclose(f);
		printf("Read %d samples from %d sensors.\n", numsamples, NUM_SENSORS);
		printf("Processed %f seconds of ADC data in %f seconds.\n",
			   (float) numsamples / ram::sonar::SAMPRATE,
			   (float) clock_run / CLOCKS_PER_SEC);
		printf("Clocking I/O...\n");
		clock_input = clock_io(argv[1]);
		printf("Spent %f seconds in disk I/O.\n",
			   (float) clock_input / CLOCKS_PER_SEC);
		printf("  ==> Analysis took approx. %f seconds.\n",
			   (float) (clock_run - clock_input) / CLOCKS_PER_SEC);
	}
	else
	{
		fprintf(stderr,"No file specified.\n");
		return -1;
	}
    return 0;
}


clock_t clock_io(char * fname)
{
	FILE *f;
	clock_t clock_run;
	ram::sonar::adcdata_t sample[NUM_SENSORS];
	int numsamples = 0;
	f = fopen(fname, "rb");
	if (f == NULL)
	{
		fprintf(stderr,"Could not open file.");
		return -1;
	}
	clock_run = clock();
	while (!feof(f))
	{
		fread(sample, sizeof(ram::sonar::adcdata_t), NUM_SENSORS, f);
		numsamples ++;
	}
	clock_run = clock() - clock_run;
	fclose(f);
    return clock_run;
}

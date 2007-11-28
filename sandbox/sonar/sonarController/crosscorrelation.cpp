/*
 *  crosscorrelation.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 11/28/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 *  Simple test program for timeOfMaxCrossCorrelation(a,b)
 *
 */

#include <iostream>
#include <stdio.h>

#include "SonarController.h"
#include "SonarChunk.h"

#define NUM_SENSORS 5

int main (int argc, char * const argv[]) {
	FILE *f;
	adcdata_t sample[NUM_SENSORS];
	int numsamples = 0;
	SonarChunk a(0), b(0);
	if (argc >= 2) {
		f = fopen(argv[1], "rb");
		if (f == NULL) {
			std::cerr << "Could not open file." << std::endl;
			return -1;
		}
		while (!feof(f) && numsamples < SonarChunk::capacity) {
			fread(sample, sizeof(adcdata_t), NUM_SENSORS, f);
			a.append(sample[0]);
			b.append(sample[1]);
			numsamples ++;
		}
		fclose(f);
		std::cout << "Read " << numsamples << " samples from " 
			<< NUM_SENSORS << " sensors." << std::endl;
		std::cout << "Index of maximum cross correlation: " 
			<< timeOfMaxCrossCorrelation(a,b) << std::endl;
	} else {
		std::cerr << "No file specified." << std::endl;
		return -1;
	}
    return 0;
}

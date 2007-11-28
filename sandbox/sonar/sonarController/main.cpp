/*
 *  main.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 11/28/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */

#include <iostream>
#include <stdio.h>

#include "SonarController.h"

#define NUM_SENSORS 5

int main (int argc, char * const argv[]) {
	FILE *f;
	SonarController sc(NUM_SENSORS);
	adcdata_t sample[NUM_SENSORS];
	int numsamples = 0;
	int numstars = 0;
	if (argc >= 2) {
		f = fopen(argv[1], "rb");
		if (f == NULL) {
			std::cerr << "Could not open file." << std::endl;
			return -1;
		}
		while (!feof(f)) {
			fread(sample, sizeof(adcdata_t), NUM_SENSORS, f);
			sc.receiveSample(sample);
			for (int sensor = 0 ; sensor < NUM_SENSORS ; sensor ++) {
				numstars = sc.getMag(sensor) / (1500000 / 12);
				for (int i = 0 ; i < numstars ; i++)
					std::cout << '#';
				for (int i = numstars - 1 ; i < 12 ; i ++)
					std::cout << ' ';
			}
			std::cout << std::endl;
			numsamples ++;
		}
		fclose(f);
		std::cout << "Read " << numsamples << " samples from " 
			<< NUM_SENSORS << " sensors." << std::endl;
	} else {
		std::cerr << "No file specified." << std::endl;
		return -1;
	}
    return 0;
}

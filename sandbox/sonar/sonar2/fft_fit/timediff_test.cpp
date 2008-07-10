// g++ -lgsl -lgslcblas quadfit_test.cpp sonar_quadfit.cpp -o quadfit_test
/*
 *  main.cpp
 *  sonarController
 *
 *  Created by Michael Levashov based on code by Leo Singer
 *  Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 */

#include <stdio.h>
#include <unistd.h>
//#include <vector>
#define MAX_SAMPLE_LENGTH 13000
#include "/home/levashom/auv/sonar/sonarController/include/Sonar.h"
#include "time_diff.h"
#define NUM_SENSORS 4

using namespace ram::sonar;

clock_t clock_io(char * fname);

int main (int argc, char * const argv[])
{
	FILE *f;
	adcdata_t sample[NUM_SENSORS];
	int numsamples = 0;
	double* data[NUM_SENSORS];
	double tdiff;
	time_diff* mytdiff;

	for(int k=0; k<NUM_SENSORS; k++)
		data[k]=new double[MAX_SAMPLE_LENGTH];

	if (argc >= 2)
	{
		f = fopen(argv[1], "rb");
		if (f == NULL)
		{
			fprintf(stderr,"Could not open file.");
			return -1;
		}
		while (!feof(f))
		{
			fread(sample, sizeof(adcdata_t), NUM_SENSORS, f);
			for(int k=0;k<NUM_SENSORS;k++)
				data[k][numsamples]=sample[k];
			numsamples ++;
		}
		numsamples--; //We reached the end of file with the previous operation
		fclose(f);

		//Now, send the data into the time_diff function
		mytdiff=new time_diff(NUM_SENSORS, numsamples, (int)30e4, (int)32e3, (int)28e3);
		if(mytdiff->update(data)!=0)
		{
			printf("Error updating the data sample\n");
			exit(1);
		}
		for(int k=0; k<NUM_SENSORS-1;k++)
			for(int j=k+1;j<NUM_SENSORS;j++)
				if(mytdiff->calc_time_diff(k,j,tdiff)!=0)
					printf("Error calculating tdiff!\n");
				else
				{
					if(tdiff>numsamples/2)
						tdiff-=numsamples;
					printf("Between %d and %d: %f points\n",k+1,j+1,tdiff);
				}

		delete mytdiff;
	}
	else
	{
		fprintf(stderr,"No file specified.\n");
		return -1;
	}
    return 0;
}

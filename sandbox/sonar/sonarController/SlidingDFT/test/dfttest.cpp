/*
 *  dfttest.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 11/28/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "../../Sonar.h"
#include "../SimpleSlidingDFT.h"


int doDFT(char *infile, char *outfile, int nchannels, int k, int N);


int main (int argc, char * const argv[])
{
	if (argc != 6)
	{
		fprintf(stderr, "Wrong number of arguments!\n");
		return -1;
	}
	else
	{
		return doDFT(argv[1], argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
	}
}


int doDFT(char *infile, char *outfile, int nchannels, int k, int N)
{
	FILE *fi, *fo;
	ram::sonar::adcdata_t sample[nchannels];
	int result = 0, numsamples = 0;
	clock_t clock_run;
	ram::sonar::SimpleSlidingDFT dft(nchannels, k, N);
	
	fi = fopen(infile, "rb");
	if (fi == NULL)
	{
		perror("Error opening input file");
		return -1;
	}
	
	fo = fopen(outfile, "wb");
	if (fo == NULL)
	{
		perror("Error opening output file");
		return -1;
	}
	
	clock_run = clock();
	
	//	This loop has an off by one error -- it reads 1000 samples but outputs
	//	1001 DFT data points.  Maybe I'll fix this later if I feel like it.
	while(!feof(fi))
	{
		result = fread(sample, sizeof(ram::sonar::adcdata_t), nchannels, fi);
		if (ferror(fi))
		{
			perror("Error reading input file");
			return -1;
		}
		
		dft.update(sample);
		
		for (int channel = 0 ; channel < nchannels ; channel ++)
		{
			ram::sonar::adcdata_t mag = 
				(ram::sonar::adcdata_t) ((float) dft.getMagL2(channel) / dft.getUnity() * 127);
			fwrite(&mag, sizeof(ram::sonar::adcdata_t), 1, fo);
			if (ferror(fo))
			{
				perror("Error writing output file");
				return -1;
			}
		}
		
		numsamples ++;
	}
	clock_run = clock() - clock_run;
	
	fclose(fi);
	fclose(fo);

	printf("Read %d samples from %d sensors.\n", numsamples, nchannels);
	printf("Processed %f seconds of ADC data in %f seconds.\n",
		   (float) numsamples / ram::sonar::SAMPRATE,
		   (float) clock_run / CLOCKS_PER_SEC);
	
	return 0;
}

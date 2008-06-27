/**
 * TestDSO.cpp
 *
 * @author Leo Singer
 * @author Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 */


#include <UnitTest++/UnitTest++.h>
#include "DSO.h"


#include <stdio.h>


using namespace ram::sonar;


struct DSOTestFixture {};


TEST_FIXTURE(DSOTestFixture, TryDSO)
{
	DSO dso(NCHANNELS, 2, 20, 20);
	FILE *f;
	char fname[] = "/home/lsinger/workspace/sonarController/test/adc_extended.dat";
	
	adcdata_t sample[NCHANNELS];
	int numsamples = 0;
	#include <stdio.h>
	
	f = fopen(fname, "rb");
	if (f == NULL)
	{
		fprintf(stderr,"Could not open file.");
		exit(-1);
	}
	
	while (!feof(f))
	{
		fread(sample, sizeof(adcdata_t), NCHANNELS, f);
		dso.update(sample);
		numsamples ++;
	}
	fclose(f);
	
	dso.thinkAboutIt();
	
	/*
	char fnamereconstructed[] = "/home/lsinger/workspace/sonarController/test/reconstructedadc.dat";
	adcdata_t *reconstructed = new adcdata_t[NCHANNELS * numsamples];
	dso.reconstruct_adcdata(reconstructed, numsamples);
	
	FILE *fr = fopen(fnamereconstructed, "wb");
	if (fr == NULL)
	{
		fprintf(stderr,"Could not open file.");
		exit(-1);
	}
	fwrite(reconstructed, sizeof(adcdata_t), NCHANNELS * numsamples, fr);
	fclose(fr);
	
	delete [] reconstructed;
	*/
}

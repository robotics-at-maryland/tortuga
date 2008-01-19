/*
 *  DSO.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 1/18/08.
 *  Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 */


#include "DSO.h"


#include "Sonar.h"
#include "SimpleSlidingDFT.h"
#include "SonarChunk.h"
#include "SampleDelay.h"


#include <stdio.h>


namespace ram {
namespace sonar {


DSO::DSO(int nchannels, int k, int N, int presample) :
	dft(nchannels, k, N), delayQueue(nchannels, presample), 
	nchannels(nchannels), k(k), N(N),
	threshold(200000),
	sample_index(0)
{
	oldChunks = new std::vector<SonarChunk*>[nchannels];
	currentChunks = new SonarChunk*[nchannels];
	triggers = new bool[nchannels];
	for (int channel = 0 ; channel < nchannels ; channel ++)
	{
		triggers[channel] = false;
		oldChunks[channel] = std::vector<SonarChunk*>();
		currentChunks[channel] = NULL;
	}
}


DSO::~DSO()
{
	delete [] triggers;
	delete [] oldChunks;
	delete [] currentChunks;
}


void DSO::update(adcdata_t *sample)
{
	dft.update(sample);
	delayQueue.writeSample(sample);
	adcdata_t *delayedSample = delayQueue.readSample();
	for (int channel = 0 ; channel < nchannels ; channel ++)
	{
		if (dft.getMagL1(channel) > threshold)
		{
			if (!triggers[channel])
			{
				triggers[channel] = true;
				currentChunks[channel] = SonarChunk::newInstance();
				currentChunks[channel]->startIndex = sample_index;
				printf("CH%2d: Starting sample record\n", channel);
			}
			bool bufferNotFull = 
				currentChunks[channel]->append(delayedSample[channel]);
			if (!bufferNotFull)
			{
				triggers[channel] = false;
				oldChunks[channel].push_back(currentChunks[channel]);
				currentChunks[channel] = NULL;
				printf("CH%2d: Stopping sample record, full\n", channel);
			}
		}
		else
		{
			if (triggers[channel])
			{
				triggers[channel] = false;
				oldChunks[channel].push_back(currentChunks[channel]);
				currentChunks[channel] = NULL;
				printf("CH%2d: Stopping sample record\n", channel);
			}
		}
	}
	sample_index ++;
}


void DSO::reconstruct_adcdata(adcdata_t *out, adcsampleindex_t until) const
{
	for (int i = 0 ; i < nchannels * until ; i ++)
		out[i] = 0;
	
	for (int channel = 0 ; channel < nchannels ; channel ++)
	{
		std::vector<SonarChunk*> &chunks = oldChunks[channel];
		for (int chunkIndex = 0 ; chunkIndex < chunks.size() ; chunkIndex ++)
		{
			SonarChunk &chunk = *chunks[chunkIndex];
			if (chunk.startIndex < until)
			{
				for (int i = 0 ; i < chunk.size() ; i ++)
				{
					int realIndex = chunk.startIndex + i;
					if (realIndex < until)
						out[nchannels * realIndex + channel] = chunk[i];
				}
			}
		}
	}
}


} // namespace sonar
} // namespace ram

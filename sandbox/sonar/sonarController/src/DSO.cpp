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
#include "TDOA.h"


#include <stdio.h>
#include <vector>


namespace ram {
namespace sonar {


DSO::DSO(int nchannels, int k, int N, int presample) :
	dft(nchannels, k, N), delayQueue(nchannels, presample), 
	nchannels(nchannels), k(k), N(N),
	threshold(120000),
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
				printf("CH%2d: Starting sample record *** %d\n", channel, sample_index);
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
				printf("CH%2d: Stopping sample record --- %d\n", channel, sample_index);
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
		for (unsigned int chunkIndex = 0 ; chunkIndex < chunks.size() ; chunkIndex ++)
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


void DSO::thinkAboutIt()
{
	using namespace std;
	
	adcsampleindex_t maxTDOA = (adcsampleindex_t) (MAX_SENSOR_SEPARATION * 2 / SPEED_OF_SOUND);
	
	vector<SonarChunk*>::iterator * it = 
		new vector<SonarChunk*>::iterator[nchannels];
	SonarChunk **slots = new SonarChunk*[nchannels];
	
	for (int channel = 0 ; channel < nchannels ; channel ++)
	{
		it[channel] = oldChunks[channel].begin();
		slots[channel] = NULL;
	}
	
	
	bool moreData = true;
	int countPings = 0;
	while (moreData)
	{
		for (int channel = 0 ; channel < nchannels && moreData ; channel ++)
		{
			if (slots[channel] == NULL)
			{
				it[channel]++;
				if (it[channel] == oldChunks[channel].end())
				{
					moreData = false;
				}
				else if ((*it[channel])->size() > 100)
				{
					slots[channel] = *(it[channel]);
					for (int channel2 = 0 ; channel2 < nchannels ; channel2 ++)
						if (channel2 != channel && slots[channel2] != NULL)
							if (abs(slots[channel]->startIndex - slots[channel]->startIndex) > maxTDOA)
								slots[channel] = NULL;
				}
			}
			bool pingDetected = true;
			for (int channel2 = 0 ; channel2 < nchannels ; channel2 ++)
				if (slots[channel2] == NULL)
					pingDetected = false;
			if (pingDetected)
			{
				printf("Ping %d detected!\n", countPings);
				localize(slots);
				for (int channel2 = 0 ; channel2 < nchannels ; channel2 ++)
					slots[channel2] = NULL;
				countPings ++;
			}
		}
	}
	delete [] slots;
	delete [] it;
}


void DSO::localize(SonarChunk **chunks)
{
	adcsampleindex_t *tdoas = new adcsampleindex_t[nchannels - 1];
	
	for (int channel = 1 ; channel < nchannels ; channel ++)
	{
		tdoas[channel - 1] = tdoa_xcorr(*(chunks[0]), *(chunks[channel]));
		printf("  TDOA d0%d = %d\n", channel, tdoas[channel - 1]);
	}
	
	delete [] tdoas;
}


} // namespace sonar
} // namespace ram

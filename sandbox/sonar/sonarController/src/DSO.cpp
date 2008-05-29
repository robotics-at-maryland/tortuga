/**
 * @file DSO.cpp
 *
 * @author Leo Singer
 * @author Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 */


#include "DSO.h"


#include "Sonar.h"
#include "dft/TiledSlidingDFT.h"
#include "SonarChunk.h"
#include "SampleDelay.h"
#include "TDOA.h"


#include <stdio.h>
#include <vector>
#include <assert.h>


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
	//  Maximum possible TDOA in units of samples
	adcsampleindex_t maxTDOA = (adcsampleindex_t) (MAX_SENSOR_SEPARATION * 2 / SPEED_OF_SOUND * SAMPRATE);
	
	adcsampleindex_t minIndex, maxIndex;
	int minIndexSlot, maxIndexSlot;
	
	std::vector<SonarChunk*>::iterator * it = 
		new std::vector<SonarChunk*>::iterator[nchannels];
	SonarChunk **slots = new SonarChunk*[nchannels];
	
	//	Grab the first SonarChunk from each channel and put it in the corresponding slot
	bool moreData = true;
	for (int channel = 0 ; channel < nchannels ; channel ++)
	{
		it[channel] = oldChunks[channel].begin();
		if (it[channel] == oldChunks[channel].end())
			moreData = false;
		else
			slots[channel] = *(it[channel]);
	}
	
	if (!moreData || nchannels <= 0)
		return;  //  You IDIOT!
	
	//  Retrieve the minimum and maximum start index from each channel.
	minIndex = slots[0]->startIndex;
	maxIndex = slots[0]->startIndex;
	minIndexSlot = 0;
	maxIndexSlot = 0;
	for (int channel = 1 ; channel < nchannels ; channel ++)
	{
		adcsampleindex_t startIndex = slots[channel]->startIndex;
		if (startIndex > maxIndex)
		{
			maxIndex = startIndex;
			maxIndexSlot = channel;
		}
		if (startIndex < minIndex)
		{
			minIndex = startIndex;
			minIndexSlot = channel;
		}
	}
	
	int countPings = 0;
	while (moreData)
	{
		if (maxIndex - minIndex > maxTDOA)
		{
			it[minIndexSlot]++;
			if (it[minIndexSlot] == oldChunks[minIndexSlot].end())
			{
				moreData = false;
				break;
			}
			
			SonarChunk *nextChunk = *(it[minIndexSlot]);
			slots[minIndexSlot] = nextChunk;
			
			minIndex = slots[0]->startIndex;
			maxIndex = slots[0]->startIndex;
			minIndexSlot = 0;
			maxIndexSlot = 0;
			for (int channel = 1 ; channel < nchannels ; channel ++)
			{
				adcsampleindex_t startIndex = slots[channel]->startIndex;
				if (startIndex > maxIndex)
				{
					maxIndex = startIndex;
					maxIndexSlot = channel;
				}
				if (startIndex < minIndex)
				{
					minIndex = startIndex;
					minIndexSlot = channel;
				}
			}
		}
		else
		{
			printf("Ping %d detected with channel 0 at sample index %d\n", countPings, slots[0]->startIndex);
			localize(slots);
			countPings ++;
			for (int channel = 0 ; channel < nchannels ; channel ++)
			{
				it[channel]++;
				if (it[channel] == oldChunks[channel].end())
					moreData = false;
				else
					slots[channel] = *(it[channel]);
			}
			if (!moreData)
				break;
			minIndex = slots[0]->startIndex;
			maxIndex = slots[0]->startIndex;
			minIndexSlot = 0;
			maxIndexSlot = 0;
			for (int channel = 1 ; channel < nchannels ; channel ++)
			{
				adcsampleindex_t startIndex = slots[channel]->startIndex;
				if (startIndex > maxIndex)
				{
					maxIndex = startIndex;
					maxIndexSlot = channel;
				}
				if (startIndex < minIndex)
				{
					minIndex = startIndex;
					minIndexSlot = channel;
				}
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

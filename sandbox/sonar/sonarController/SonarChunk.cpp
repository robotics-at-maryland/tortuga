/*
 *  SonarChunk.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 11/28/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */


#include "SonarChunk.h"


#include <math.h>
#include <string.h>
#include <algorithm>


SonarChunk::SonarChunk(adcsampleindex_t si)
{
	startIndex = si;
	sample = new adcdata_t[capacity];
	purge();
}

SonarChunk::~SonarChunk()
{
	delete [] sample;
}


bool SonarChunk::append(adcdata_t datum)
{
	if (length <= capacity)
	{
		sample[length++] = datum;
		if (abs(datum) > peak)
			peak = datum;
		return true;
	}
	else
	{
		return false;
	}
}


int SonarChunk::size() const
{
	return length;
}


adcdata_t SonarChunk::getPeak() const
{
	return peak;
}


const adcdata_t &SonarChunk::operator[](adcsampleindex_t i) const
{
	return sample[i];
}


void SonarChunk::purge()
{
	length = 0;
	peak = 0;
}


adcsampleindex_t timeOfMaxCrossCorrelation(const SonarChunk &a, const SonarChunk &b)
{
	adcmath_t max = 0, accum = 0;
	int maxindex = 0;
	for (int k = -b.size() ; k < a.size() ; k ++)
	{
		accum = 0;
		for (int i = std::max(0, std::min(k, a.size())) ; i < std::min(a.size(), k + b.size()) ; i ++)
			accum += a[i] + b[i + k];
		if (accum > max)
		{
			max = accum;
			maxindex = k;
		}
	}	
	return maxindex - a.startIndex + b.startIndex;
}

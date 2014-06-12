/**
 * @file SonarChunk.cpp
 *
 * @author Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */


#include "SonarChunk.h"


#include <math.h>
#include <string.h>
#include <algorithm>


namespace ram {
namespace sonar {


const int SonarChunk::capacity(2048);
std::list<SonarChunk*> SonarChunk::pool;


SonarChunk::SonarChunk()
{
	sample = new adcdata_t[capacity];
	purge();
}


SonarChunk::~SonarChunk()
{ delete [] sample; }


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


adcsampleindex_t SonarChunk::size() const
{ return length; }


adcdata_t SonarChunk::getPeak() const { return peak; }

const adcdata_t &SonarChunk::operator[](adcsampleindex_t i) const
{ return sample[i]; }


void SonarChunk::setFourierComponents(adcmath_t re, adcmath_t im)
{
	fourierAmpReal = re;
	fourierAmpImag = im;
	phase = atan2f(im, re);
}


adcmath_t SonarChunk::getFourierAmpReal() const { return fourierAmpReal; }

adcmath_t SonarChunk::getFourierAmpImag() const { return fourierAmpImag; }

float SonarChunk::getPhase() const { return phase; }


void SonarChunk::purge()
{
	length = 0;
	peak = 0;
	startIndex = 0;
	phase = 0;
	startIndex = 0;
	setFourierComponents(0, 0);
}


void SonarChunk::recycle()
{
	purge();
	pool.push_front(this);
}


SonarChunk *SonarChunk::newInstance()
{
	if (SonarChunk::pool.empty())
	{
		return new SonarChunk();
	}
	else
	{
		SonarChunk *nextInstance = pool.front();
		pool.pop_front();
		return nextInstance;
	}
}


void SonarChunk::emptyPool() { emptyPool(0); }


void SonarChunk::emptyPool(adcsampleindex_t numToRemain)
{
	while (pool.size() > (size_t)numToRemain)
	{
		delete pool.front();
		pool.pop_front();
	}
}


} // namespace sonar
} // namespace ram

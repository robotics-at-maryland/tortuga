/*
 *  TestTDOAxcorr.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 1/21/08.
 *  Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 */


#include <UnitTest++/UnitTest++.h>
#include <stdlib.h>
#include <math.h>


#include "SonarChunk.h"
#include "TDOA.h"


using namespace ram::sonar;


struct TDOAxcorrTestFixture {
	
	static SonarChunk *makeTrianglePulse(adcsampleindex_t centerIndex, adcsampleindex_t size, adcsampleindex_t startIndex, float slope)
	{
		SonarChunk *sc = SonarChunk::newInstance();
		sc->startIndex = startIndex;
		for (adcsampleindex_t i = 0 ; i < size; i ++)
		{
			sc->append((adcdata_t) (-slope * abs(i - centerIndex)));
		}
		return sc; 
	}
	
	static SonarChunk *makeTopHatPulse(adcsampleindex_t startPulse, adcsampleindex_t stopPulse, adcsampleindex_t size, adcsampleindex_t offset, adcdata_t amplitude)
	{
		SonarChunk *sc = SonarChunk::newInstance();
		sc->startIndex = offset;
		for (adcsampleindex_t i = 0 ; i < size ; i ++)
		{
			if (i >= startPulse && i <= stopPulse)
				sc->append(amplitude);
			else
				sc->append(0);
		}
		return sc;
	}
	
};


TEST_FIXTURE(TDOAxcorrTestFixture, ZeroDelayNoOffset)
{
	SonarChunk *a = makeTopHatPulse(100,200,400,500,20);
	SonarChunk *b = makeTopHatPulse(100,200,400,500,20);
	CHECK_EQUAL(0, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}


TEST_FIXTURE(TDOAxcorrTestFixture, PositiveDelayPositiveOffset)
{
	SonarChunk *a = makeTopHatPulse(100,200,400,500,20);
	SonarChunk *b = makeTopHatPulse(100,200,400,600,20);
	CHECK_EQUAL(100, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}


TEST_FIXTURE(TDOAxcorrTestFixture, NegativeDelayNegativeOffset)
{
	SonarChunk *a = makeTopHatPulse(100,200,400,600,20);
	SonarChunk *b = makeTopHatPulse(100,200,400,500,20);
	CHECK_EQUAL(-100, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}


TEST_FIXTURE(TDOAxcorrTestFixture, PositiveDelayNoOffset)
{
	SonarChunk *a = makeTopHatPulse(100,200,400,500,20);
	SonarChunk *b = makeTopHatPulse(200,300,400,500,20);
	CHECK_EQUAL(100, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}


TEST_FIXTURE(TDOAxcorrTestFixture, NegativeDelayNoOffset)
{
	SonarChunk *a = makeTopHatPulse(200,300,400,500,20);
	SonarChunk *b = makeTopHatPulse(100,200,400,500,20);
	CHECK_EQUAL(-100, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}


TEST_FIXTURE(TDOAxcorrTestFixture, NoDelayPositiveOffset)
{
	SonarChunk *a = makeTopHatPulse(200,300,400,500,20);
	SonarChunk *b = makeTopHatPulse(100,200,400,600,20);
	CHECK_EQUAL(0, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}


TEST_FIXTURE(TDOAxcorrTestFixture, NoDelayNegativeOffset)
{
	SonarChunk *a = makeTopHatPulse(100,200,400,600,20);
	SonarChunk *b = makeTopHatPulse(200,300,400,500,20);
	CHECK_EQUAL(0, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}


/**********************************************/
/** first SonarChunk shorter than the second **/
/**********************************************/


TEST_FIXTURE(TDOAxcorrTestFixture, ZeroDelayNoOffsetFirstShorter)
{
	SonarChunk *a = makeTopHatPulse(100,200,400,500,20);
	SonarChunk *b = makeTopHatPulse(100,200,600,500,20);
	CHECK_EQUAL(0, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}


TEST_FIXTURE(TDOAxcorrTestFixture, PositiveDelayPositiveOffsetFirstShorter)
{
	SonarChunk *a = makeTopHatPulse(100,200,400,500,20);
	SonarChunk *b = makeTopHatPulse(100,200,600,600,20);
	CHECK_EQUAL(100, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}


TEST_FIXTURE(TDOAxcorrTestFixture, NegativeDelayNegativeOffsetFirstShorter)
{
	SonarChunk *a = makeTopHatPulse(100,200,400,600,20);
	SonarChunk *b = makeTopHatPulse(100,200,600,500,20);
	CHECK_EQUAL(-100, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}


TEST_FIXTURE(TDOAxcorrTestFixture, PositiveDelayNoOffsetFirstShorter)
{
	SonarChunk *a = makeTopHatPulse(100,200,400,500,20);
	SonarChunk *b = makeTopHatPulse(200,300,600,500,20);
	CHECK_EQUAL(100, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}


TEST_FIXTURE(TDOAxcorrTestFixture, NegativeDelayNoOffsetFirstShorter)
{
	SonarChunk *a = makeTopHatPulse(200,300,400,500,20);
	SonarChunk *b = makeTopHatPulse(100,200,600,500,20);
	CHECK_EQUAL(-100, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}


TEST_FIXTURE(TDOAxcorrTestFixture, NoDelayPositiveOffsetFirstShorter)
{
	SonarChunk *a = makeTopHatPulse(200,300,400,500,20);
	SonarChunk *b = makeTopHatPulse(100,200,600,600,20);
	CHECK_EQUAL(0, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}


TEST_FIXTURE(TDOAxcorrTestFixture, NoDelayNegativeOffsetFirstShorter)
{
	SonarChunk *a = makeTopHatPulse(100,200,400,600,20);
	SonarChunk *b = makeTopHatPulse(200,300,600,500,20);
	CHECK_EQUAL(0, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}


/*********************************************/
/** first SonarChunk longer than the second **/
/*********************************************/


TEST_FIXTURE(TDOAxcorrTestFixture, ZeroDelayNoOffsetFirstLonger)
{
	SonarChunk *a = makeTopHatPulse(100,200,600,500,20);
	SonarChunk *b = makeTopHatPulse(100,200,400,500,20);
	CHECK_EQUAL(0, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}


TEST_FIXTURE(TDOAxcorrTestFixture, PositiveDelayPositiveOffsetFirstLonger)
{
	SonarChunk *a = makeTopHatPulse(100,200,600,500,20);
	SonarChunk *b = makeTopHatPulse(100,200,400,600,20);
	CHECK_EQUAL(100, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}


TEST_FIXTURE(TDOAxcorrTestFixture, NegativeDelayNegativeOffsetFirstLonger)
{
	SonarChunk *a = makeTopHatPulse(100,200,600,600,20);
	SonarChunk *b = makeTopHatPulse(100,200,400,500,20);
	CHECK_EQUAL(-100, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}


TEST_FIXTURE(TDOAxcorrTestFixture, PositiveDelayNoOffsetFirstLonger)
{
	SonarChunk *a = makeTopHatPulse(100,200,600,500,20);
	SonarChunk *b = makeTopHatPulse(200,300,400,500,20);
	CHECK_EQUAL(100, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}


TEST_FIXTURE(TDOAxcorrTestFixture, NegativeDelayNoOffsetFirstLonger)
{
	SonarChunk *a = makeTopHatPulse(200,300,600,500,20);
	SonarChunk *b = makeTopHatPulse(100,200,400,500,20);
	CHECK_EQUAL(-100, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}


TEST_FIXTURE(TDOAxcorrTestFixture, NoDelayPositiveOffsetFirstLonger)
{
	SonarChunk *a = makeTopHatPulse(200,300,600,500,20);
	SonarChunk *b = makeTopHatPulse(100,200,400,600,20);
	CHECK_EQUAL(0, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}


TEST_FIXTURE(TDOAxcorrTestFixture, NoDelayNegativeOffsetFirstLonger)
{
	SonarChunk *a = makeTopHatPulse(100,200,600,600,20);
	SonarChunk *b = makeTopHatPulse(200,300,400,500,20);
	CHECK_EQUAL(0, tdoa_xcorr(*a, *b));
	a->recycle();
	b->recycle();
	SonarChunk::emptyPool();
}

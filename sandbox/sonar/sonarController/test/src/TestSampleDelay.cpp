/*
 *  TestSampleDelay.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 1/8/08.
 *  Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 */


#include "SampleDelay.h"


#include <UnitTest++/UnitTest++.h>


struct SampleDelayTestFixture {
};


TEST_FIXTURE(SampleDelayTestFixture, Delay4)
{
	using namespace ram::sonar;
	adcdata_t sample0[] = {0, 1, 2};
	adcdata_t sample1[] = {3, 4, 5};
	adcdata_t sample2[] = {6, 7, 8};
	adcdata_t sample3[] = {9, 10, 11};
	adcdata_t sample4[] = {12, 13, 14};
	adcdata_t sample5[] = {15, 16, 17};
	SampleDelay delay(2, 4);
	delay.writeSample(sample0);
	delay.writeSample(sample1);
	delay.writeSample(sample2);
	delay.writeSample(sample3);
	delay.writeSample(sample4);
	delay.writeSample(sample5);
}

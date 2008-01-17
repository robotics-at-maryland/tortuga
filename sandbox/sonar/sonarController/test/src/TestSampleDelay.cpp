/*
 *  TestSampleDelay.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 1/8/08.
 *  Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 */


#include <UnitTest++/UnitTest++.h>
#include <stdint.h>


#include "SampleDelay.h"


struct SampleDelayTestFixture {
};


TEST_FIXTURE(SampleDelayTestFixture, DelayCopyByValue)
{
	using namespace ram::sonar;
	SampleDelay delay(3,0);
	adcdata_t v1[] = {20,21,22};
	delay.writeSample(v1);
	adcdata_t v2[] = {20,21,22};
	CHECK_ARRAY_EQUAL(delay.readSample(),v2,3);
}


TEST_FIXTURE(SampleDelayTestFixture, Delay4)
{
	using namespace ram::sonar;
	adcdata_t sampleEmpty[] = {0, 0, 0};
	adcdata_t sample0[] = {0, 1, 2};
	adcdata_t sample1[] = {3, 4, 5};
	adcdata_t sample2[] = {6, 7, 8};
	adcdata_t sample3[] = {9, 10, 11};
	adcdata_t sample4[] = {12, 13, 14};
	adcdata_t sample5[] = {15, 16, 17};
	adcdata_t sample6[] = {18, 19, 20};
	SampleDelay delay(3, 4);
	CHECK_ARRAY_EQUAL(delay.readSample(), sampleEmpty, 3);
	delay.writeSample(sample0);
	CHECK_ARRAY_EQUAL(delay.readSample(), sampleEmpty, 3);
	delay.writeSample(sample1);
	CHECK_ARRAY_EQUAL(delay.readSample(), sampleEmpty, 3);
	delay.writeSample(sample2);
	CHECK_ARRAY_EQUAL(delay.readSample(), sampleEmpty, 3);
	delay.writeSample(sample3);
	CHECK_ARRAY_EQUAL(delay.readSample(), sampleEmpty, 3);
	delay.writeSample(sample4);
	CHECK_ARRAY_EQUAL(delay.readSample(), sample0, 3);
	delay.writeSample(sample5);
	CHECK_ARRAY_EQUAL(delay.readSample(), sample1, 3);
	delay.writeSample(sample6);
	CHECK_ARRAY_EQUAL(delay.readSample(), sample2, 3);
	CHECK_ARRAY_EQUAL(delay.readSample(), sample2, 3);
}


TEST_FIXTURE(SampleDelayTestFixture, PointerArithmeticAmICrazy)
{
	int8_t arr_8bit[] = {32,33,34,35,36};
	int16_t arr_16bit[] = {37,38,39,40,41};
	int32_t arr_32bit[] = {41,42,43,44,45};
	CHECK_EQUAL(35, *(arr_8bit + 3));
	CHECK_EQUAL(40, *(arr_16bit + 3));
	CHECK_EQUAL(44, *(arr_32bit + 3));
}


TEST_FIXTURE(SampleDelayTestFixture, Delay0)
{
	using namespace ram::sonar;
	adcdata_t sampleEmpty[] = {0, 0, 0};
	adcdata_t sample0[] = {0, 1, 2};
	adcdata_t sample1[] = {3, 4, 5};
	adcdata_t sample2[] = {6, 7, 8};
	adcdata_t sample3[] = {9, 10, 11};
	SampleDelay delay(3,0);
	CHECK_ARRAY_EQUAL(delay.readSample(), sampleEmpty, 3);
	CHECK_ARRAY_EQUAL(delay.readSample(), sampleEmpty, 3);
	delay.writeSample(sample0);
	CHECK_ARRAY_EQUAL(delay.readSample(), sample0, 3);
	delay.writeSample(sample1);
	CHECK_ARRAY_EQUAL(delay.readSample(), sample1, 3);
	delay.writeSample(sample2);
	CHECK_ARRAY_EQUAL(delay.readSample(), sample2, 3);
	delay.writeSample(sample3);
	CHECK_ARRAY_EQUAL(delay.readSample(), sample3, 3);
}

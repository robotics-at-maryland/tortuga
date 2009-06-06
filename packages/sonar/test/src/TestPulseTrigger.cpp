/**
 * TestPulseTrigger.cpp
 *
 * @author Leo Singer
 * @author Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 */

#include <UnitTest++/UnitTest++.h>
#include <cstdlib>

#include "PulseTrigger.h"

using namespace ram::sonar;
using namespace std;

SUITE(TestPulseTrigger)
{
	TEST(DualIntChannel)
	{
		static const int N = 10;
		static const int nChannels = 2;
		PulseTrigger<int, N, nChannels> pt(100);
		int signal[nChannels];
		signal[0] = signal[1] = 99;
		
		for (int i = 0 ; i < 2*N ; i ++)
		{
			pt.update(signal);
			CHECK(!pt(0));
			CHECK(!pt(1));
		}
		signal[0] = 100;
		for (int i = 0 ; i < N-1 ; i ++)
		{
			pt.update(signal);
			CHECK(!pt(0));
			CHECK(!pt(1));
		}
		pt.update(signal);
		CHECK(pt(0));
		CHECK(!pt(1));
		signal[0] = -99;
		pt.update(signal);
		CHECK(!pt(0));
		CHECK(!pt(1));
	}
}

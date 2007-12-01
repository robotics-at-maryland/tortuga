/*
 *  TDOA.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 11/30/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */


#include "TDOA.h"


#include <algorithm>
#include <math.h>


adcsampleindex_t tdoa_xcorr(const SonarChunk &a, const SonarChunk &b)
{
	long int max = 0, accum = 0;
	adcsampleindex_t maxindex = 0;
	for (int k = -b.size() ; k < a.size() ; k ++)
	{
		accum = 0;
		for (int i = std::max(0, std::min(k, a.size())) ; i < std::min(a.size(), k + b.size()) ; i ++)
			accum += a[i] * b[i + k];
		if (accum > max)
		{
			max = accum;
			maxindex = k;
		}
	}	
	return maxindex - a.startIndex + b.startIndex;
}


adcsampleindex_t tdoa_phase(const SonarChunk &a, const SonarChunk &b)
{
	//	TODO insert code here
	return 0;
}

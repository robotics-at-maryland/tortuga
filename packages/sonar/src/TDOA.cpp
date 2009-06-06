/**
 * @file TDOA.cpp
 *
 * @author Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */


#include "TDOA.h"


#include <algorithm>
#include <math.h>
#include <limits.h>
#include <assert.h>


namespace ram {
namespace sonar {


adcsampleindex_t tdoa_xcorr(const SonarChunk &f, const SonarChunk &g)
{
	long int max = LONG_MIN;
	adcsampleindex_t maxindex = 0;
	
	for (int k = -f.size() ; k < g.size() ; k ++)
	{
		long int accum = 0;
		
		int gMinIndex = std::max(0, k);
		
		int gMaxIndex = g.size();
		if (gMaxIndex > k + f.size())
			gMaxIndex = k + f.size();
		
		for (int i = gMinIndex ; i < gMaxIndex ; i ++)
		{
			assert(i >= 0);
			assert(i - k >= 0);
			assert(i < g.size());
			assert(i - k < f.size());
			accum += f[i - k] * g[i];
		}
		if (accum > max)
		{
			max = accum;
			maxindex = k;
		}
	}
	return maxindex - f.startIndex + g.startIndex;
}


} // namespace sonar
} // namespace ram

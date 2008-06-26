/**
 * @file fixed.h
 *
 * @author Alex Golden
 * @author Copyright 2008 Robotics@Maryland. All rights reserved.
 * 
 */

#ifndef _RAM_FIXED_H
#define _RAM_FIXED_H

namespace ram { namespace sonar{ namespace fixed{

extern "C" long PseudoPolarize(long *argx, long *argy);

inline long fixed_atan2(const long &x, const long &y)
{
	return PseudoPolarize(&x, &y);
}

}}}  // namespace ram::sonar::fixed

#endif

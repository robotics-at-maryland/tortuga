/**
 * @file fixed.h
 *
 * @author Alex Golden
 * @author Copyright 2008 Robotics@Maryland. All rights reserved.
 * 
 */

#ifndef _RAM_FIXED_H
#define _RAM_FIXED_H

#include <complex>
#include <cmath>

namespace ram { namespace sonar { namespace fixed{

typedef std::complex<int64_t> complex_int64;

inline int64_t int64_abs(const int64_t &x)
	{ return (x < 0 ? -x : x); }

inline int64_t magL1(const complex_int64& a)
{
	return int64_abs(a.real()) + int64_abs(a.imag());
}

inline int64_t magL2(const complex_int64& a)
{
	return (int64_t) std::sqrt((double)a.real()*a.real() + (double)a.imag()*a.imag());
}

inline double phaseBetween(const complex_int64& a, const complex_int64& b)
{
	std::complex<double> aDbl((double)a.real(), (double)a.imag());
	std::complex<double> bDbl((double)b.real(), (double)b.imag());
	return arg(aDbl*conj(bDbl));
}

extern "C" long PseudoPolarize(long *argx, long *argy);

inline long fixed_atan2(long &x, long &y)
{
	return PseudoPolarize(&x, &y);
}

}}}  // namespace ram::sonar::fixed

#endif

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

namespace ram { namespace sonar { namespace fixed {

typedef std::complex<int64_t> complex_int64;

template<typename T>
inline T abs(const T& x)
{ return (x < 0 ? -x : x); }

template<typename T>
inline T magL1(const std::complex<T>& a)
{ return abs(a.real()) + abs(a.imag()); }

template<typename T>
inline T magL2(const std::complex<T>& a)
{
	return (T)std::sqrt((double)a.real()*a.real() + (double)a.imag()*a.imag());
}

template<typename T>
inline double phaseBetween(const std::complex<T>& a, const std::complex<T>& b)
{
	std::complex<double> aDbl((double)a.real(), (double)a.imag());
	std::complex<double> bDbl((double)b.real(), (double)b.imag());
	return arg(aDbl*conj(bDbl));
}

template<typename T>
inline T round(const double& a)
{
    return (T)std::floor(a+0.5);
}

extern "C" long PseudoPolarize(long *argx, long *argy);

inline long fixed_atan2(long &x, long &y)
{
	return PseudoPolarize(&x, &y);
}

}}}  // namespace ram::sonar::fixed

#endif

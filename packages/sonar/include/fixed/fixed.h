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

template<typename T>
inline T sign(const T& x)
{
    if (x < 0)
        return -1;
    else if (x > 0)
        return 1;
    else
        return 0;
}

template<typename T>
inline T abs(const T& x)
{ return (x < 0 ? -x : x); }

/**
 * Computes the L1 norm of a complex number a+bi as |a|+|b|.
 */
template<typename T>
inline T magL1(const std::complex<T>& a)
{ return abs(a.real()) + abs(a.imag()); }

/**
 * Computes the L2 norm of a complex number a+bi as sqrt(a^2+b^2).
 */
template<typename T>
inline T magL2(const std::complex<T>& a)
{
	return (T)std::sqrt((double)a.real()*a.real() + (double)a.imag()*a.imag());
}

/**
 * Calculates the relative phase between two complex numbers.
 */
template<typename T>
inline double phaseBetween(const std::complex<T>& a, const std::complex<T>& b)
{
	std::complex<double> aDbl((double)a.real(), (double)a.imag());
	std::complex<double> bDbl((double)b.real(), (double)b.imag());
	return arg(aDbl*conj(bDbl));
}

/**
 * Rounds a double to the nearest integer.
 * @warning the template type must be an integer or unsigned integer data type.
 */
template<typename T>
inline T round(const double& a)
{
    return (T)(a+0.5*sign(a));
}

/**
 * Truncate an integer in binary by \a whichBit places, 
 * rounding to the (\a whichBit + 1) least significant bit.
 *
 * For example, if \a whichBit is 1, then the return value will have 1 less 
 * significant bit than the input \a a.
 */
template<typename T>
inline T truncateWithRound(const T& a, short unsigned whichBit)
{
    if (a >= 0)
        return (a + ((T)1 << (whichBit - 1))) >> whichBit;
    else
        return -((-a + ((T)1 << (whichBit - 1))) >> whichBit);
}


extern "C" long PseudoPolarize(long *argx, long *argy);

inline long fixed_atan2(long &x, long &y)
{
	return PseudoPolarize(&x, &y);
}

}}}  // namespace ram::sonar::fixed

#endif

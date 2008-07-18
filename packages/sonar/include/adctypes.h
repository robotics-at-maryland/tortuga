/**
 * @file adctypes.h
 * Generic datatypes for fixed-with integers
 *
 * @author Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 * 
 */

#ifndef _RAM_SONAR_ADCTYPES_H
#define _RAM_SONAR_ADCTYPES_H

#include <stdint.h>
#include <limits.h>

namespace ram { namespace sonar {

namespace { // hide messy internals in an anonymous namespace
	template<int bitRange>
	struct adctype1
	{
	};

	template<>
	struct adctype1<0> {
		typedef int8_t SIGNED;
		typedef uint8_t UNSIGNED;
	};

	template<>
	struct adctype1<1> {
		typedef int16_t SIGNED;
		typedef uint16_t UNSIGNED;
	};

	template<>
	struct adctype1<2> {
		typedef int32_t SIGNED;
		typedef int32_t UNSIGNED;
	};

	template<>
	struct adctype1<3> {
		typedef int64_t SIGNED;
		typedef uint64_t UNSIGNED;
	};
}

template<int _bitDepth>
struct adctype
{
private:
	static const int _bitRange = 
	(_bitDepth <= 8 ? 0 : 
	 (_bitDepth <= 16 ? 1 : 
	  (_bitDepth <= 32 ? 2 : 
	   (_bitDepth <= 64 ? 3 : 4))));
	static const uint64_t UINT64_MAX = ULLONG_MAX;
public:
	typedef typename adctype1<_bitRange>::SIGNED SIGNED;
	typedef typename adctype1<_bitRange>::UNSIGNED UNSIGNED;
	typedef struct adctype<1*_bitDepth> SINGLE_WIDE;
	typedef struct adctype<2*_bitDepth> DOUBLE_WIDE;
	typedef struct adctype<4*_bitDepth> QUADRUPLE_WIDE;
	
	static const int BITDEPTH = _bitDepth;
	static const SIGNED SIGNED_MIN = -((int64_t)1 << (_bitDepth - 1));
	static const SIGNED SIGNED_MAX =  ((int64_t)1 << (_bitDepth - 1)) - 1;
	static const UNSIGNED UNSIGNED_MAX = ((~((uint64_t)0)) >> (64 - _bitDepth));
};

}} // namespace ram::sonar

#endif

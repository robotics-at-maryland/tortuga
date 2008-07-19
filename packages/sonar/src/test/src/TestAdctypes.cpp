/**
 * TestAdctypes.cpp
 *
 * @author Leo Singer
 * @author Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 */

//	Enable fixed with limit macros
#define __STDC_LIMIT_MACROS

#include <UnitTest++/UnitTest++.h>
#include <cstdlib>
#include <stdint.h>

#include "adctypes.h"

using namespace ram::sonar;
using namespace std;

SUITE(TestAdctypes)
{
	
	TEST(TestMaxUnsigned)
	{
		CHECK_EQUAL(3,          adctype<2>::UNSIGNED_MAX());
		CHECK_EQUAL(7,          adctype<3>::UNSIGNED_MAX());
		CHECK_EQUAL(15,         adctype<4>::UNSIGNED_MAX());
		CHECK_EQUAL(31,         adctype<5>::UNSIGNED_MAX());
		CHECK_EQUAL(63,         adctype<6>::UNSIGNED_MAX());
		CHECK_EQUAL(127,        adctype<7>::UNSIGNED_MAX());
		CHECK_EQUAL(255,        adctype<8>::UNSIGNED_MAX());
		CHECK_EQUAL(511,        adctype<9>::UNSIGNED_MAX());
		CHECK_EQUAL(1023,       adctype<10>::UNSIGNED_MAX());
		CHECK_EQUAL(UINT16_MAX, adctype<16>::UNSIGNED_MAX());
		CHECK_EQUAL(UINT32_MAX, adctype<32>::UNSIGNED_MAX());
		CHECK_EQUAL(UINT64_MAX, adctype<64>::UNSIGNED_MAX());
	}
	
	TEST(TestMaxSigned)
	{
		CHECK_EQUAL(1,          adctype<2>::SIGNED_MAX());
		CHECK_EQUAL(3,          adctype<3>::SIGNED_MAX());
		CHECK_EQUAL(7,          adctype<4>::SIGNED_MAX());
		CHECK_EQUAL(15,         adctype<5>::SIGNED_MAX());
		CHECK_EQUAL(31,         adctype<6>::SIGNED_MAX());
		CHECK_EQUAL(63,         adctype<7>::SIGNED_MAX());
		CHECK_EQUAL(127,        adctype<8>::SIGNED_MAX());
		CHECK_EQUAL(255,        adctype<9>::SIGNED_MAX());
		CHECK_EQUAL(511,        adctype<10>::SIGNED_MAX());
		CHECK_EQUAL(INT16_MAX,  adctype<16>::SIGNED_MAX());
		CHECK_EQUAL(INT32_MAX,  adctype<32>::SIGNED_MAX());
		CHECK_EQUAL(INT64_MAX,  adctype<64>::SIGNED_MAX());
	}
	
	TEST(TestMinSigned)
	{
		CHECK_EQUAL(-2,         adctype<2>::SIGNED_MIN());
		CHECK_EQUAL(-4,         adctype<3>::SIGNED_MIN());
		CHECK_EQUAL(-8,         adctype<4>::SIGNED_MIN());
		CHECK_EQUAL(-16,        adctype<5>::SIGNED_MIN());
		CHECK_EQUAL(-32,        adctype<6>::SIGNED_MIN());
		CHECK_EQUAL(-64,        adctype<7>::SIGNED_MIN());
		CHECK_EQUAL(-128,       adctype<8>::SIGNED_MIN());
		CHECK_EQUAL(-256,       adctype<9>::SIGNED_MIN());
		CHECK_EQUAL(-512,       adctype<10>::SIGNED_MIN());
		CHECK_EQUAL(INT16_MIN,  adctype<16>::SIGNED_MIN());
		CHECK_EQUAL(INT32_MIN,  adctype<32>::SIGNED_MIN());
		CHECK_EQUAL(INT64_MIN,  adctype<64>::SIGNED_MIN());
	}
	
	TEST(TestBitDepth)
	{
		CHECK_EQUAL(30, adctype<15>::DOUBLE_WIDE::BITDEPTH());
		CHECK_EQUAL(60, adctype<15>::QUADRUPLE_WIDE::BITDEPTH());
		CHECK_EQUAL(63, adctype<63>::SINGLE_WIDE::BITDEPTH());
		CHECK_EQUAL(4, adctype<1>::QUADRUPLE_WIDE::BITDEPTH());
	}
	
	TEST(TestMachineSize)
	{
		CHECK_EQUAL(sizeof(int8_t), sizeof(adctype<8>::SINGLE_WIDE::SIGNED));
		CHECK_EQUAL(sizeof(int16_t), sizeof(adctype<16>::SINGLE_WIDE::SIGNED));
		CHECK_EQUAL(sizeof(int32_t), sizeof(adctype<32>::SINGLE_WIDE::SIGNED));
		CHECK_EQUAL(sizeof(int64_t), sizeof(adctype<64>::SINGLE_WIDE::SIGNED));
		
		CHECK_EQUAL(sizeof(int16_t), sizeof(adctype<8>::DOUBLE_WIDE::SIGNED));
		CHECK_EQUAL(sizeof(int32_t), sizeof(adctype<8>::QUADRUPLE_WIDE::SIGNED));
		CHECK_EQUAL(sizeof(int64_t), sizeof(adctype<16>::QUADRUPLE_WIDE::SIGNED));
	}
	
}

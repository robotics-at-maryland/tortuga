/**
 * Testadcs.cpp
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

SUITE(Testadcs)
{
	
	TEST(TestMaxUnsigned)
	{
		CHECK_EQUAL(3,          adc<2>::UNSIGNED_MAX());
		CHECK_EQUAL(7,          adc<3>::UNSIGNED_MAX());
		CHECK_EQUAL(15,         adc<4>::UNSIGNED_MAX());
		CHECK_EQUAL(31,         adc<5>::UNSIGNED_MAX());
		CHECK_EQUAL(63,         adc<6>::UNSIGNED_MAX());
		CHECK_EQUAL(127,        adc<7>::UNSIGNED_MAX());
		CHECK_EQUAL(255,        adc<8>::UNSIGNED_MAX());
		CHECK_EQUAL(511,        adc<9>::UNSIGNED_MAX());
		CHECK_EQUAL(1023,       adc<10>::UNSIGNED_MAX());
		CHECK_EQUAL(UINT16_MAX, adc<16>::UNSIGNED_MAX());
		CHECK_EQUAL(UINT32_MAX, adc<32>::UNSIGNED_MAX());
		CHECK_EQUAL(UINT64_MAX, adc<64>::UNSIGNED_MAX());
	}
	
	TEST(TestMaxSigned)
	{
		CHECK_EQUAL(1,          adc<2>::SIGNED_MAX());
		CHECK_EQUAL(3,          adc<3>::SIGNED_MAX());
		CHECK_EQUAL(7,          adc<4>::SIGNED_MAX());
		CHECK_EQUAL(15,         adc<5>::SIGNED_MAX());
		CHECK_EQUAL(31,         adc<6>::SIGNED_MAX());
		CHECK_EQUAL(63,         adc<7>::SIGNED_MAX());
		CHECK_EQUAL(127,        adc<8>::SIGNED_MAX());
		CHECK_EQUAL(255,        adc<9>::SIGNED_MAX());
		CHECK_EQUAL(511,        adc<10>::SIGNED_MAX());
		CHECK_EQUAL(INT16_MAX,  adc<16>::SIGNED_MAX());
		CHECK_EQUAL(INT32_MAX,  adc<32>::SIGNED_MAX());
		CHECK_EQUAL(INT64_MAX,  adc<64>::SIGNED_MAX());
	}
	
	TEST(TestMinSigned)
	{
		CHECK_EQUAL(-2,         adc<2>::SIGNED_MIN());
		CHECK_EQUAL(-4,         adc<3>::SIGNED_MIN());
		CHECK_EQUAL(-8,         adc<4>::SIGNED_MIN());
		CHECK_EQUAL(-16,        adc<5>::SIGNED_MIN());
		CHECK_EQUAL(-32,        adc<6>::SIGNED_MIN());
		CHECK_EQUAL(-64,        adc<7>::SIGNED_MIN());
		CHECK_EQUAL(-128,       adc<8>::SIGNED_MIN());
		CHECK_EQUAL(-256,       adc<9>::SIGNED_MIN());
		CHECK_EQUAL(-512,       adc<10>::SIGNED_MIN());
		CHECK_EQUAL(INT16_MIN,  adc<16>::SIGNED_MIN());
		CHECK_EQUAL(INT32_MIN,  adc<32>::SIGNED_MIN());
		CHECK_EQUAL(INT64_MIN,  adc<64>::SIGNED_MIN());
	}
	
	TEST(TestBitDepth)
	{
		CHECK_EQUAL(30, adc<15>::DOUBLE_WIDE::BITDEPTH());
		CHECK_EQUAL(60, adc<15>::QUADRUPLE_WIDE::BITDEPTH());
		CHECK_EQUAL(63, adc<63>::SINGLE_WIDE::BITDEPTH());
		CHECK_EQUAL(4, adc<1>::QUADRUPLE_WIDE::BITDEPTH());
	}
	
	TEST(TestMachineSize)
	{
		CHECK_EQUAL(sizeof(int8_t), sizeof(adc<8>::SINGLE_WIDE::SIGNED));
		CHECK_EQUAL(sizeof(int16_t), sizeof(adc<16>::SINGLE_WIDE::SIGNED));
		CHECK_EQUAL(sizeof(int32_t), sizeof(adc<32>::SINGLE_WIDE::SIGNED));
		CHECK_EQUAL(sizeof(int64_t), sizeof(adc<64>::SINGLE_WIDE::SIGNED));
		
		CHECK_EQUAL(sizeof(int16_t), sizeof(adc<8>::DOUBLE_WIDE::SIGNED));
		CHECK_EQUAL(sizeof(int32_t), sizeof(adc<8>::QUADRUPLE_WIDE::SIGNED));
		CHECK_EQUAL(sizeof(int64_t), sizeof(adc<16>::QUADRUPLE_WIDE::SIGNED));
	}
	
}

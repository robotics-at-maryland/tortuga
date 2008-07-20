/**
 * TestAdctypes.cpp
 *
 * @author Leo Singer
 * @author Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 */

//	Enable fixed with limit macros
#define __STDC_LIMIT_MACROS

#include "adctypes.h"

#include <UnitTest++/UnitTest++.h>
#include <cstdlib>
#include <stdint.h>

using namespace ram::sonar;
using namespace std;

SUITE(TestAdctypes)
{

	TEST(TestMaxUnsigned)
	{
		{
			adc<2>::UNSIGNED result = adc<2>::UNSIGNED_MAX;
			CHECK_EQUAL(3, result);
		}
		{
			adc<3>::UNSIGNED result = adc<3>::UNSIGNED_MAX;
			CHECK_EQUAL(7, result);
		}
		{
			adc<4>::UNSIGNED result = adc<4>::UNSIGNED_MAX;
			CHECK_EQUAL(15, result);
		}
		{
			adc<5>::UNSIGNED result = adc<5>::UNSIGNED_MAX;
			CHECK_EQUAL(31, result);
		}
		{
			adc<6>::UNSIGNED result = adc<6>::UNSIGNED_MAX;
			CHECK_EQUAL(63, result);
		}
		{
			adc<7>::UNSIGNED result = adc<7>::UNSIGNED_MAX;
			CHECK_EQUAL(127, result);
		}
		{
			adc<8>::UNSIGNED result = adc<8>::UNSIGNED_MAX;
			CHECK_EQUAL(255, result);
		}
		{
			adc<9>::UNSIGNED result = adc<9>::UNSIGNED_MAX;
			CHECK_EQUAL(511, result);
		}
		{
			adc<10>::UNSIGNED result = adc<10>::UNSIGNED_MAX;
			CHECK_EQUAL(1023, result);
		}
		{
			adc<16>::UNSIGNED result = adc<16>::UNSIGNED_MAX;
			CHECK_EQUAL(UINT16_MAX, result);
		}
		{
			adc<32>::UNSIGNED result = adc<32>::UNSIGNED_MAX;
			CHECK_EQUAL(UINT32_MAX, result);
		}
		{
			adc<64>::UNSIGNED result = adc<64>::UNSIGNED_MAX;
			CHECK_EQUAL(UINT64_MAX, result);
		}
	}
	
	TEST(TestMaxSigned)
	{
		{
			adc<2>::SIGNED result = adc<2>::SIGNED_MAX;
			CHECK_EQUAL(1, result);
		}
		{
			adc<3>::SIGNED result = adc<3>::SIGNED_MAX;
			CHECK_EQUAL(3, result);
		}
		{
			adc<4>::SIGNED result = adc<4>::SIGNED_MAX;
			CHECK_EQUAL(7, result);
		}
		{
			adc<5>::SIGNED result = adc<5>::SIGNED_MAX;
			CHECK_EQUAL(15, result);
		}
		{
			adc<6>::SIGNED result = adc<6>::SIGNED_MAX;
			CHECK_EQUAL(31, result);
		}
		{
			adc<7>::SIGNED result = adc<7>::SIGNED_MAX;
			CHECK_EQUAL(63, result);
		}
		{
			adc<8>::SIGNED result = adc<8>::SIGNED_MAX;
			CHECK_EQUAL(127, result);
		}
		{
			adc<9>::SIGNED result = adc<9>::SIGNED_MAX;
			CHECK_EQUAL(255, result);
		}
		{
			adc<10>::SIGNED result = adc<10>::SIGNED_MAX;
			CHECK_EQUAL(511, result);
		}
		{
			adc<16>::SIGNED result = adc<16>::SIGNED_MAX;
			CHECK_EQUAL(INT16_MAX, result);
		}
		{
			adc<32>::SIGNED result = adc<32>::SIGNED_MAX;
			CHECK_EQUAL(INT32_MAX, result);
		}
		{
			adc<64>::SIGNED result = adc<64>::SIGNED_MAX;
			CHECK_EQUAL(INT64_MAX, result);
		}
	}
	
	TEST(TestMinSigned)
	{
		{
			adc<2>::SIGNED result = adc<2>::SIGNED_MIN;
			CHECK_EQUAL(-2, result);
		}
		{
			adc<3>::SIGNED result = adc<3>::SIGNED_MIN;
			CHECK_EQUAL(-4, result);
		}
		{
			adc<4>::SIGNED result = adc<4>::SIGNED_MIN;
			CHECK_EQUAL(-8, result);
		}
		{
			adc<5>::SIGNED result = adc<5>::SIGNED_MIN;
			CHECK_EQUAL(-16, result);
		}
		{
			adc<6>::SIGNED result = adc<6>::SIGNED_MIN;
			CHECK_EQUAL(-32, result);
		}
		{
			adc<7>::SIGNED result = adc<7>::SIGNED_MIN;
			CHECK_EQUAL(-64, result);
		}
		{
			adc<8>::SIGNED result = adc<8>::SIGNED_MIN;
			CHECK_EQUAL(-128, result);
		}
		{
			adc<9>::SIGNED result = adc<9>::SIGNED_MIN;
			CHECK_EQUAL(-256, result);
		}
		{
			adc<10>::SIGNED result = adc<10>::SIGNED_MIN;
			CHECK_EQUAL(-512, result);
		}
		{
			adc<16>::SIGNED result = adc<16>::SIGNED_MIN;
			CHECK_EQUAL(INT16_MIN, result);
		}
		{
			adc<32>::SIGNED result = adc<32>::SIGNED_MIN;
			CHECK_EQUAL(INT32_MIN, result);
		}
		{
			adc<64>::SIGNED result = adc<64>::SIGNED_MIN;
			CHECK_EQUAL(INT64_MIN, result);
		}
	}
	
	TEST(TestBitDepth)
	{
		{
			int result = adc<15>::DOUBLE_WIDE::BITDEPTH;
			CHECK_EQUAL(30, result);
		}
		{
			int result = adc<15>::QUADRUPLE_WIDE::BITDEPTH;
			CHECK_EQUAL(60, result);
		}
		{
			int result = adc<63>::SINGLE_WIDE::BITDEPTH;
			CHECK_EQUAL(63, result);
		}
		{
			int result = adc<1>::QUADRUPLE_WIDE::BITDEPTH;
			CHECK_EQUAL(4, result);
		}
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

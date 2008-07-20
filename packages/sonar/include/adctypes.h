/**
 * @file adctypes.h
 * Generic datatypes for fixed-with integers
 *
 * @author Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 * 
 * How to use:
 * 
 *   Choose the bit depth of your ADC -- say, 4 bits.
 *   To declare variables that stores ADC values, do the following:
 *
 *     adc<4>::SIGNED myNum;     // equivalent to char
 *     adc<4>::UNSIGNED myNum2;  // equivalent to unsigned char
 *   
 *   You may want to have another data type for numbers that are twice as wide
 *   as your ADC for intermediate calculations.
 *
 *     adc<4>::DOUBLE_WIDE::SIGNED myNum3;    // equivalent to char
 *     adc<4>::DOUBLE_WIDE::UNSIGNED myNum4;  // equivalent to unsigned char
 *
 *   Or, you might even want a quadruple wide type.
 *   
 *     adc<4>::QUADRUPLE_WIDE::SIGNED myNum5;   // equivalent to short on most platforms
 *     adc<4>::QUADRUPLE_WIDE::UNSIGNED myNum6; // equivalent to unsigned short on most platforms
 *   
 *   You don't have to limit yourself to bit depths that are multiples of two.
 *
 *     adc<3>::SIGNED myNum7;   // char
 *     adc<22>::DOUBLE_WIDE::UNSIGNED myNum8;    // unsigned int
 *
 *   If you request a type that cannot be created on your machine, then an error
 *   will be generated.
 *
 *     adc<32>::SIGNED myNum9;                    // works
 *     adc<32>::DOUBLE_WIDE::SIGNED myNum10;      // works
 *     adc<32>::QUADRUPLE_WIDE::SIGNED myNum11;   // FAILS
 *     adc<33>::SIGNED myNum12;                   // works
 *     adc<33>::DOUBLE_WIDE::SIGNED myNum13;      // FAILS
 *     adc<128>::SIGNED myNum14;                  // FAILS
 *
 *   You can get useful information about your ADC-specific types;
 *
 *     adc<5>::SIGNED_MIN   // -32
 *     adc<5>::SIGNED_MAX   // 31
 *     adc<5>::DOUBLE_WIDE::UNSIGNED_MAX  // 1023
 *     adc<5>::QUADRUPLE_WIDE::BITDEPTH   // 20
 *     adc<5>::QUADRUPLE_WIDE::MACHINE_SIZE  // 4 (sizeof(int))
 *
 *
 *   Caveat: if you use adc inside a template, you may have to precede
 *   variable declarations with 'typename':
 *
 *     template<typename ADC>
 *     class MyFilter
 *     {
 *     public:
 *       MyFilter();
 *       ~MyFilter();
 *       // A function that returns a signed quadruple-wide number
 *       typename ADC::QUADRUPLE_WIDE::SIGNED foo();
 *     private:
 *       // A single-wide signed variable initialized to 42
 *       typename ADC::SIGNED mySigned = 42;
 *       // A 20-element array of double-wide unsigned numbers
 *       typename ADC::DOUBLE_WIDE::UNSIGNED myArray[20];
 *       // A constant (no typename necessary)
 *       size_t someSize = 3 * ADC::DOUBLE_WIDE::MACHINE_SIZE;
 *     };
 *
 */

#ifndef _RAM_SONAR_adcS_H
#define _RAM_SONAR_adcS_H

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

namespace ram { namespace sonar {

namespace { // hide messy internals in an anonymous namespace
	template<int bitRange>
	struct adc1
	{
	};

	template<>
	struct adc1<0> {
		typedef int8_t SIGNED;
		typedef uint8_t UNSIGNED;
	};

	template<>
	struct adc1<1> {
		typedef int16_t SIGNED;
		typedef uint16_t UNSIGNED;
	};

	template<>
	struct adc1<2> {
		typedef int32_t SIGNED;
		typedef uint32_t UNSIGNED;
	};

	template<>
	struct adc1<3> {
		typedef int64_t SIGNED;
		typedef uint64_t UNSIGNED;
	};
}

template<int _bitDepth>
struct adc
{
private:
	static const int _bitRange = 
	(_bitDepth <= 8 ? 0 : 
	 (_bitDepth <= 16 ? 1 : 
	  (_bitDepth <= 32 ? 2 : 
	   (_bitDepth <= 64 ? 3 : 4))));
public:
	typedef typename adc1<_bitRange>::SIGNED SIGNED;
	typedef typename adc1<_bitRange>::UNSIGNED UNSIGNED;
	typedef struct adc<1*_bitDepth> SINGLE_WIDE;
	typedef struct adc<2*_bitDepth> DOUBLE_WIDE;
	typedef struct adc<4*_bitDepth> QUADRUPLE_WIDE;
	typedef struct adc<1+_bitDepth> ONE_MORE_BIT;
	
	static const int BITDEPTH = _bitDepth;
	static const SIGNED SIGNED_MIN = -((int64_t)1 << (_bitDepth - 1));
	static const SIGNED SIGNED_MAX = ((int64_t)1 << (_bitDepth - 1)) - 1;
	static const UNSIGNED UNSIGNED_MAX = ((~((uint64_t)0)) >> (64 - _bitDepth));
	static const size_t MACHINE_SIZE = sizeof(SIGNED);
};

}} // namespace ram::sonar

#endif

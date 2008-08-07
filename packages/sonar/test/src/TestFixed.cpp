/**
 * TestFixed.cpp
 *
 * @author Leo Singer
 * @author Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 */

#include <UnitTest++/UnitTest++.h>
#include <cstdlib>
#include <stdint.h>

#include "fixed/fixed.h"

using namespace ram::sonar;
using namespace std;

SUITE(TestFixed)
{
    TEST(RoundInt)
    {
        CHECK_EQUAL(1,   fixed::round<int>(0.5));
        CHECK_EQUAL(0,   fixed::round<int>(0.49999));
        CHECK_EQUAL(0,   fixed::round<int>(0.0));
        CHECK_EQUAL(21,  fixed::round<int>(21.2));
        CHECK_EQUAL(22,  fixed::round<int>(21.5));
        CHECK_EQUAL(22,  fixed::round<int>(21.7));
        CHECK_EQUAL(22,  fixed::round<int>(21.999999));
        CHECK_EQUAL(0,   fixed::round<int>(-0.4));
        CHECK_EQUAL(0,   fixed::round<int>(-0.499999999999999));
        CHECK_EQUAL(-1,  fixed::round<int>(-0.5));
        CHECK_EQUAL(-1,  fixed::round<int>(-0.7));
        CHECK_EQUAL(-21, fixed::round<int>(-21.0));
        CHECK_EQUAL(-21, fixed::round<int>(-21.2));
        CHECK_EQUAL(-21, fixed::round<int>(-21.4999999));
        CHECK_EQUAL(-22, fixed::round<int>(-21.5));
        CHECK_EQUAL(-22, fixed::round<int>(-21.7));
        CHECK_EQUAL(-22, fixed::round<int>(-21.99999999));
    }
    
    TEST(RoundUnsignedInt)
    {
        CHECK_EQUAL((unsigned)1,   fixed::round<unsigned>(0.5));
        CHECK_EQUAL((unsigned)0,   fixed::round<unsigned>(0.49999));
        CHECK_EQUAL((unsigned)0,   fixed::round<unsigned>(0.0));
        CHECK_EQUAL((unsigned)21,  fixed::round<unsigned>(21.2));
        CHECK_EQUAL((unsigned)22,  fixed::round<unsigned>(21.5));
        CHECK_EQUAL((unsigned)22,  fixed::round<unsigned>(21.7));
        CHECK_EQUAL((unsigned)22,  fixed::round<unsigned>(21.999999));
    }
    
    TEST(TruncateWithRoundOnePlace)
    {
        CHECK_EQUAL(2,  fixed::truncateWithRound(4,  1)); //  0b0100 -->  0b0010
        CHECK_EQUAL(3,  fixed::truncateWithRound(5,  1)); //  0b0101 -->  0b0011
        CHECK_EQUAL(3,  fixed::truncateWithRound(6,  1)); //  0b0110 -->  0b0011
        CHECK_EQUAL(1,  fixed::truncateWithRound(1,  1)); //  0b0001 -->  0b0001
        CHECK_EQUAL(0,  fixed::truncateWithRound(0,  1)); //  0b0000 -->  0b0000
        CHECK_EQUAL(-2, fixed::truncateWithRound(-4, 1)); // -0b0100 --> -0b0010
        CHECK_EQUAL(-3, fixed::truncateWithRound(-5, 1)); // -0b0101 --> -0b0011
        CHECK_EQUAL(-3, fixed::truncateWithRound(-6, 1)); // -0b0110 --> -0b0011
        CHECK_EQUAL(-1, fixed::truncateWithRound(-1, 1)); // -0b0001 --> -0b0001
    }
    
    TEST(TruncateWithRoundTwoPlaces)
    {
        CHECK_EQUAL(2,  fixed::truncateWithRound(6,  2)); //  0b0110 -->  0b0010
        CHECK_EQUAL(1,  fixed::truncateWithRound(5,  2)); //  0b0101 -->  0b0001
        CHECK_EQUAL(1,  fixed::truncateWithRound(4,  2)); //  0b0100 -->  0b0001
        CHECK_EQUAL(1,  fixed::truncateWithRound(3,  2)); //  0b0011 -->  0b0001
        CHECK_EQUAL(1,  fixed::truncateWithRound(2,  2)); //  0b0010 -->  0b0001
        CHECK_EQUAL(0,  fixed::truncateWithRound(1,  2)); //  0b0001 -->  0b0000
        CHECK_EQUAL(0,  fixed::truncateWithRound(0,  2)); //  0b0000 -->  0b0000
        CHECK_EQUAL(-2, fixed::truncateWithRound(-6, 2)); // -0b0110 --> -0b0010
        CHECK_EQUAL(-1, fixed::truncateWithRound(-5, 2)); // -0b0101 --> -0b0001
        CHECK_EQUAL(-1, fixed::truncateWithRound(-4, 2)); // -0b0100 --> -0b0001
        CHECK_EQUAL(-1, fixed::truncateWithRound(-3, 2)); // -0b0011 --> -0b0001
        CHECK_EQUAL(-1, fixed::truncateWithRound(-2, 2)); // -0b0010 --> -0b0001
        CHECK_EQUAL(0,  fixed::truncateWithRound(-1, 2)); // -0b0001 -->  0b0000
    }
    
    TEST(AbsoluteValue)
    {
        CHECK_EQUAL(1,  fixed::abs(1));
        CHECK_EQUAL(0,  fixed::abs(0));
        CHECK_EQUAL(1,  fixed::abs(-1));
        CHECK_EQUAL(21, fixed::abs(21));
        CHECK_EQUAL(21, fixed::abs(-21));
    }
}

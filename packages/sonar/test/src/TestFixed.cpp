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
    
    /*
    TEST(RoundToBit)
    {
        CHECK_EQUAL(2,  fixed::roundToBit(4, 1));
        CHECK_EQUAL(3,  fixed::roundToBit(5, 1));
        CHECK_EQUAL(3,  fixed::roundToBit(6, 1));
        CHECK_EQUAL(1,  fixed::roundToBit(1, 1));
        CHECK_EQUAL(0,  fixed::roundToBit(0, 1));
        CHECK_EQUAL(-2, fixed::roundToBit(-4, 1));
        CHECK_EQUAL(-3, fixed::roundToBit(-5, 1));
        CHECK_EQUAL(-3, fixed::roundToBit(-6, 1));
        CHECK_EQUAL(-1, fixed::roundToBit(-1, 1));
    }
    */
        
    TEST(AbsoluteValue)
    {
        CHECK_EQUAL(1,  fixed::abs(1));
        CHECK_EQUAL(0,  fixed::abs(0));
        CHECK_EQUAL(1,  fixed::abs(-1));
        CHECK_EQUAL(21, fixed::abs(21));
        CHECK_EQUAL(21, fixed::abs(-21));
    }
}

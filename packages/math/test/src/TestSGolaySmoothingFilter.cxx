/*
 * Copyright (C) 2010 Robotics at Maryland
 * All rights reserved.
 *
 * Author: Jonathan Wonders
 * File:  packages/math/include/SGolaySmoothingFilter.cxx
 */

// STD Includes
#include <iostream>

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "math/include/SGolaySmoothingFilter.h"
#include "math/include/MatrixN.h"
#include "math/include/VectorN.h"

TEST(SGolaySmoothingFilter)
{
    int windowSize = 5;
    int degree = 2;

    ram::math::SGolaySmoothingFilter filter(windowSize, degree);

    ram::math::MatrixN coeff = filter.getCoefficientMatrix();
    ram::math::VectorN exp(0.0, windowSize);

    CHECK_EQUAL(coeff.getRows(), degree + 1);
    CHECK_EQUAL(coeff.getCols(), windowSize);
        
    exp[0] = 0.08571;
    exp[1] = -0.14286;
    exp[2] = -0.08571;
    exp[3] = 0.25714;
    exp[4] = 0.88571;

    // check the first row
    for(int col = 0; col < windowSize; col++)
        CHECK_CLOSE(exp[col], coeff[0][col], 0.0001);

    filter.addValue(1);
    
    CHECK_CLOSE(0.88571, filter.getValue(), 0.0001);

    filter.addValue(1);
    filter.addValue(1);
    filter.addValue(1);
    filter.addValue(1);
    
    CHECK_CLOSE(1.0, filter.getValue(), 0.0001);
    
    // make sure values are added properly
    filter.addValue(2);
    
    CHECK_CLOSE(1.88571, filter.getValue(), 0.0001);


    filter.addValue(2);
    filter.addValue(2);
    filter.addValue(2);
    filter.addValue(2);

    CHECK_CLOSE(2, filter.getValue(), 0.0001);
}

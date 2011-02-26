/*
 * Copyright (C) 2010 Robotics at Maryland
 * All rights reserved.
 *
 * Author: Jonathan Wonders
 * File:  packages/math/src/SGolaySmoothingFilter.cpp
 */

// STD Includes
#include <cassert>
#include <cmath>
#include <iostream>

// Library Includes

// Project Includes
#include "math/include/SGolaySmoothingFilter.h"
#include "math/include/MatrixN.h"
#include "math/include/Helpers.h"

namespace ram {
namespace math {

SGolaySmoothingFilter::SGolaySmoothingFilter(int size, int degree)
    : m_windowSize(size + (size + 1) % 2),
      m_degree(degree),
      m_values(VectorN(0.0, degree)),
      m_windowData(VectorN(0.0, m_windowSize)),
      m_coeffMatrix(MatrixN(0.0, m_degree + 1, m_windowSize))
{
    // initialize matrix of coefficients
    MatrixN designMatrix(1.0, m_windowSize, m_degree + 1);

    // generate a vector of the intervals across the window, centered at zero, 
    // each vector is raised to the power of its column index. note the first 
    // column is skipped because a^0 is always 1 and the matrix has been 
    // initialized to 1.

    for(int row = 0; row < m_windowSize; row++) {
        for(int col = 1; col < m_degree + 1; col++) {
            double base = row - (m_windowSize - 1);
            designMatrix[row][col] = std::pow(base, col);
        }
    }

    MatrixN JxJT = (designMatrix.transpose() * designMatrix);
    m_coeffMatrix = JxJT.inverse() * designMatrix.transpose();
}

SGolaySmoothingFilter::~SGolaySmoothingFilter()
{
}

void SGolaySmoothingFilter::addValue(double newValue)
{
    // shifts elements foreward in the array
    for(int i = 0; i < m_windowSize-1; i++)
        m_windowData[i] = m_windowData[i+1];

    // insert the new data at the end of the array
    m_windowData[m_windowSize-1] = newValue;

    // take the linear combination of the data with calculated coefficients
    m_values = m_coeffMatrix * m_windowData;
}

double SGolaySmoothingFilter::getValue(int order, double timestep)
{
    if(order == 0) {
        return m_values[order];
    } else if(order > 0 && order < m_degree && timestep > 0) {
        // multiply the value of given order by the coefficient due
        // to taking the derivative of a polynomial
        double tpow = std::pow(timestep, order);
        double dcoeff = math::factorial(order - 1);
        return tpow * dcoeff * m_values[order];
    }
    
    return 0;
}

int SGolaySmoothingFilter::getWindowSize()
{
    return m_windowSize;
}

int SGolaySmoothingFilter::getDegree()
{
    return m_degree;
}

void SGolaySmoothingFilter::clear()
{
    assert(0 && "Not Yet Implemented");
}

MatrixN SGolaySmoothingFilter::getCoefficientMatrix()
{
    return m_coeffMatrix;
}

} // namespace math
} // namespace ram

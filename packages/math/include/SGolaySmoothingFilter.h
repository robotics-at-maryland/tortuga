/*
 * Copyright (C) 2010 Robotics at Maryland
 * All rights reserved.
 *
 * Author: Jonathan Wonders
 * File:  packages/core/include/SGolaySmoothingFilter.h
 */


#ifndef RAM_MATH_SGOLAYSMOOTHINGFILTER_H
#define RAM_MATH_SGOLAYSMOOTHINGFILTER_H

// STD Includes

// Library Includes
#include <boost/shared_ptr.hpp>

// Project Includes
#include "math/include/MatrixN.h"
#include "math/include/VectorN.h"

namespace ram {
namespace math {

class SGolaySmoothingFilter;
typedef boost::shared_ptr<SGolaySmoothingFilter> SGolaySmoothingFilterPtr;

class SGolaySmoothingFilter
{
public:
    /** Construct a Savitzky-Golay smoothing filter 

        The Savitzky-Golay filter is a digital filter that can be applied
        to a window of equally spaced sample points.  It is a type of low
        pass filter so it will filter out high frequency noise.  It is also
        sometimes called a least-squares filter because it performs a least
        squares polynomial fit on the data within the window.  The advantage
        of this filter is that the coefficients of the fit are independent
        of the data.  They only require knowledge of the polynomial degree
        and the window size.

        @param size - the window size of the filter (must be odd)
        @param degree - the polynomial degree of the least-squares fit

     */
    SGolaySmoothingFilter(int size, int degree);

    ~SGolaySmoothingFilter();

    /** Put a new value into the filter (throws the oldest off) */
    void addValue(double newValue);

    /** Returns the window size for the filter */
    int getWindowSize();

    /** Returns the polynomial degree for the least squares fit */
    int getDegree();

    /** Gets the value of the filter of given order derivative */
    double getValue(int order = 0, double timestep = 0);

    /** Returns the matrix of coefficients for the filter */
    MatrixN getCoefficientMatrix();

    /** Clears all values in the filter */
    void clear();

private:
    int m_windowSize;
    int m_degree;

    VectorN m_values;

    VectorN m_windowData;

    // a matrix of linear combination coefficients 
    // each row represents a derivative order corresponding to its index
    MatrixN m_coeffMatrix;
};

} // namespace math
} // namespace ram

#endif // RAM_MATH_SGOLAYSMOOTHINGFILTER_H

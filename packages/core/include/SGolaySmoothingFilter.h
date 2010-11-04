/*
 * Copyright (C) 2010 Robotics at Maryland
 * All rights reserved.
 *
 * Author: Jonathan Wonders
 * File:  packages/core/include/SGolaySmoothingFilter.h
 */


#ifndef RAM_CORE_SGOLAYSMOOTHINGFILTER_H
#define RAM_CORE_SGOLAYSMOOTHINGFILTER_H

namespace ram {
namespace core {

template <class T, int DEGREE, int SIZE>
class SGolaySmoothingFilter
{
public:
    SGolaySmoothingFilter() : size(0),
                              windowSize(SIZE),
                              degree(DEGREE),
                              start(0),
                              value(0)
    {
        for(int i = 0; i < windowSize; i++)
            array[i] = 0.0;

        // Calculate the coefficients here
        

    }

    /** Put a new value into the filter (throws the oldest off) */
    void addValue(T newValue)
    {
        // add the value to the array at the right place

        // do the fit keeping in mind where each value is in the array
        // with respect to time

        // store the value so we dont need to calculate it multiple times
        // when someone wants to retrieve it

        // increment the size if necessary

    }

    /** Gets the value of the filter */
    T getValue()
    {
        return value;
    }

    /** Returns the current number of data values in the filter */
    int getSize()
    {
        return size;
    }
    
    /** Returns the window size for the filter */
    int getWindowSize()
    {
        return windowSize;
    }

    /** Returns the polynomial degree for the least squares fit */
    int getDegree()
    {
        return degree;
    }

    /** Clears all values in the filter */
    void clear();

private:
    int size;
    int windowSize;
    int degree;
    int start;
    T value;
    T array[SIZE];
    double coefficients[DEGREE + 1];
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_SGOLAYSMOOTHINGFILTER_H

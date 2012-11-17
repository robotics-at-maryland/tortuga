/*
 * Copyright (C) 2007 Robotics at Maryland
 * All rights reserved.
 *
 * Author: Daniel Hakim
 * File:  packages/math/include/AveragingFilter.h
 */

#ifndef RAM_MATH_AVERAGINGFILTER_H_06_30_2007
#define RAM_MATH_AVERAGINGFILTER_H_06_30_2007

// STD Includes

// Library Includes
#include <boost/shared_ptr.hpp>

// Project Includes

namespace ram {
namespace math {


/** Computes the running average of the given data
 */
template <class T, int SIZE>
class AveragingFilter
{
public:
    AveragingFilter() : size(0), maxSize(SIZE), start(0), total(0.0)
    {
        for (int i = 0 ; i < maxSize ; ++i)
            array[i] = 0.0;
    }
    
    AveragingFilter(T initialValue) :
        size(0),
        maxSize(SIZE),
        start(0),
        total(initialValue)
    {
        // Initial the array
        for (int i = 0; i < maxSize; ++i)
            array[i] = initialValue;
    };

    /** Put a new value into the filter (throws the oldest off) */
    void addValue(T newValue)
    {
        total -= array[start];
        array[start] = newValue;
        total += newValue;
        
        ++start;
        ++size;
        // This check is required, as otherwise size will spin around become
        // negative, and we'll start spewing garbage values
        if (size > maxSize)
            size = maxSize;
        if (start == maxSize)
            start = 0;
    }
    
    /** Gets the value of the fitler */
    T getValue()
    {
        if (size < maxSize)
            return total / size;
        else
            return total / maxSize;
    }

    int getSize()
    {
        return size;
    }

    /** Clears all values in the filter */
    void clear()
    {
        size = 0;
    }
    
private:
    int size; 
    int maxSize;
    int start;
    T total;
    T array[SIZE];
};

} // namespace math
} // namespace ram

#endif // RAM_MATH_AVERAGINGFILTER_H_06_30_2007

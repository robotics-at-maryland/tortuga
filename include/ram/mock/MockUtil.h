#ifndef _RAM_MOCK_MOCKUTIL_H
#define _RAM_MOCK_MOCKUTIL_H

#include <ram.h>
#include <stdlib.h>

template<type T>
randUpTo(T max)
{ return (T) ((double)rand() / RAND_MAX * max); }

template<type T>
randDownTo(T min)
{ return (T) ((double)rand() / RAND_MAX * min); }

template<type T>
randBetween(T min, T max)
{ return (T) ((double)rand() / RAND_MAX * (max - min) + min); }

template<type T>
randPlusMinus(T max)
{ return randBetween(-min, -max); }

::ram::math::Vector3Ptr randVector(double max = 1000)
{
    return new ::ram::math::Vector3(randPlusMinus(max), randPlusMinus(max), randPlusMinus(max));
}

::ram::math::QuaternionPtr randQuat(double max = 1000)
{
    return new ::ram::math::Quaternion(randPlusMinus(max), randPlusMinus(max), randPlusMinus(max), randPlusMinus(max));
}

#endif

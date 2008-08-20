#ifndef _RAM_MOCK_MOCKUTIL_H
#define _RAM_MOCK_MOCKUTIL_H

#include <ram.h>
#include <stdlib.h>
#include "math/include/Vector3.h"

namespace ram {
    namespace mock {
        
        template<typename T>
        T randUpTo(T max)
        { return (T) ((double)rand() / RAND_MAX * max); }
        
        template<typename T>
        T randDownTo(T min)
        { return (T) ((double)rand() / RAND_MAX * min); }
        
        template<typename T>
        T randBetween(T min, T max)
        { return (T) ((double)rand() / RAND_MAX * (max - min) + min); }
        
        template<typename T>
        T randPlusMinus(T max)
        { return randBetween(-max, max); }
        
        ::ram::math::Vector3Ptr randVector(double max = 1000)
        {
            return new ::ram::math::impl::Vector3(randPlusMinus(max), randPlusMinus(max), randPlusMinus(max));
        }
        
        ::ram::math::QuaternionPtr randQuat(double max = 1000)
        {
            return new ::ram::math::Quaternion(randPlusMinus(max), randPlusMinus(max), randPlusMinus(max), randPlusMinus(max));
        }
    }
}

#endif

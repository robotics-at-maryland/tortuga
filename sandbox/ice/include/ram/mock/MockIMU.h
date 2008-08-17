#ifndef _RAM_MOCK_MOCKIMU_H
#define _RAM_MOCK_MOCKIMU_H

#include <ram.h>
#include <stdlib.h>
#include "MockUtil.h"

namespace ram {
    class MockIMU : virtual public vehicle::IIMU
    {
    public:
        virtual math::Vector3Ptr getLinearAcceleration(const ::Ice::Current&c)
        { return randVector(); }
        
        virtual math::Vector3Ptr getAngularRate(const ::Ice::Current&c)
        { return randVector(); }
        
        virtual math::QuaternionPtr getOrientation(const ::Ice::Current&c)
        { return randQuat(); }
    };
}

#endif

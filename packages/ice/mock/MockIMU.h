#ifndef _RAM_MOCK_MOCKIMU_H
#define _RAM_MOCK_MOCKIMU_H

#include <ram_ice.h>
#include "MockUtil.h"
#include "MockDevice.h"

namespace ram {
    namespace mock {
        class MockIMU : virtual public vehicle::IMU, public MockDevice
        {
        public:
            MockIMU(const std::string& name) : MockDevice(name) {}
            
            virtual transport::Vector3Ptr getLinearAcceleration(const ::Ice::Current&c)
            { return randVector(); }
            
            virtual transport::Vector3Ptr getAngularRate(const ::Ice::Current&c)
            { return randVector(); }
            
            virtual transport::QuaternionPtr getOrientation(const ::Ice::Current&c)
            { return randQuat(); }
        };
    }
}

#endif

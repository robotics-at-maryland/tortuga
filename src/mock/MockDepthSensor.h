#ifndef _RAM_MOCK_MOCKDEPTHSENSOR_H
#define _RAM_MOCK_MOCKDEPTHSENSOR_H

#include <ram.h>
#include "MockUtil.h"
#include "MockDevice.h"

namespace ram {
    namespace mock {
        class MockDepthSensor : virtual public vehicle::DepthSensor, public MockDevice
        {
        public:
            MockDepthSensor(const std::string& name) : MockDevice(name) {}
            
            virtual double getDepth(const ::Ice::Current&c)
            { return randUpTo(25); }
        };
    }
}

#endif

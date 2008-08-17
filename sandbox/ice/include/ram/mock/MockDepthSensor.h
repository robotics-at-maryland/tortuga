#ifndef _RAM_MOCK_MOCKDEPTHSENSOR_H
#define _RAM_MOCK_MOCKDEPTHSENSOR_H

#include <ram.h>
#include "MockUtil.h"

namespace ram {
    namespace mock {
        class MockDepthSensor : virtual public vehicle::IDepthSensor
        {
        private:
            std::string name;
        public:
            MockDepthSensor(std::string mName) : name(mName) {}
            
            virtual std::string getName(const ::Ice::Current&c)
            { return name; }
            
            virtual double getDepth(const ::Ice::Current&c)
            { return randUpTo(25); }
        };
    }
}

#endif

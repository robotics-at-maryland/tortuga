#ifndef _RAM_MOCK_MOCKIMU_H
#define _RAM_MOCK_MOCKIMU_H

#include <ram.h>
#include <stdlib.h>
#include "MockUtil.h"

namespace ram {
    namespace mock {
        class MockIMU : virtual public vehicle::IIMU
        {
        private:
            std::string name;
        public:
            MockIMU(std::string mName) : name(mName) {}
            
            virtual std::string getName(const ::Ice::Current&c)
            { return name; }
            
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

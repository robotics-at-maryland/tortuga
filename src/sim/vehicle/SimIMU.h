#ifndef _RAM_SIM_SIMIMU_H
#define _RAM_SIM_SIMIMU_H

#include <ram.h>

namespace ram {
    namespace sim {
        class SimIMU : virtual public vehicle::IIMU
        {
        private:
            std::string name;
        public:
            SimIMU(std::string mName) : name(mName) {}
            
            virtual std::string getName(const ::Ice::Current&c)
            { return name; }
            
            virtual transport::Vector3Ptr getLinearAcceleration(const ::Ice::Current&c)
            { return new math::Vector3; /* TODO */ }
            
            virtual transport::Vector3Ptr getAngularRate(const ::Ice::Current&c)
            { return new math::Vector3; /* TODO */ }
            
            virtual transport::QuaternionPtr getOrientation(const ::Ice::Current&c)
            { return new math::Quaternion; /* TODO */ }
        };
    }
}

#endif

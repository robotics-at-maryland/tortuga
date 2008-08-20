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
            
            virtual math::Vector3Ptr getLinearAcceleration(const ::Ice::Current&c)
            { return new math::impl::Vector3; /* TODO */ }
            
            virtual math::Vector3Ptr getAngularRate(const ::Ice::Current&c)
            { return new math::impl::Vector3; /* TODO */ }
            
            virtual math::QuaternionPtr getOrientation(const ::Ice::Current&c)
            { return new math::Quaternion; /* TODO */ }
        };
    }
}

#endif

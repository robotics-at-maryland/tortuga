#ifndef _RAM_SIM_SIMIMU_H
#define _RAM_SIM_SIMIMU_H

#include <ram.h>
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"
#include "SimDevice.h"

namespace ram {
    namespace sim {
        class SimIMU : virtual public vehicle::IIMU, public SimDevice
        {
        public:
            SimIMU(std::string mName) : SimDevice(mName) {}
            
            virtual transport::Vector3Ptr getLinearAcceleration(const ::Ice::Current&c)
            { return new math::Vector3; /* TODO */ }
            
            virtual transport::Vector3Ptr getAngularRate(const ::Ice::Current&c)
            { return new math::Vector3; /* TODO */ }
            
            virtual transport::QuaternionPtr getOrientation(const ::Ice::Current&c)
            { return new math::Quaternion; /* TODO */ }
            
            virtual void stepSimulation(SimWorld& world, btScalar timeStep)
            { /* TODO */ }
        };
    }
}

#endif

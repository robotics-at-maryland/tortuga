#ifndef _RAM_SIM_SIMDEPTHSENSOR_H
#define _RAM_SIM_SIMDEPTHSENSOR_H

#include <ram.h>

#include "SimDevice.h"

namespace ram {
    namespace sim {
        class SimDepthSensor : virtual public vehicle::IDepthSensor, public SimDevice
        {
        public:
            SimDepthSensor(std::string mName) : SimDevice(mName) {}
            
            virtual double getDepth(const ::Ice::Current&c)
            { return 0; /* TODO */ }
            
            virtual void stepSimulation(SimWorld& world, btScalar timeStep)
            { /* TODO */ }
        };
    }
}

#endif

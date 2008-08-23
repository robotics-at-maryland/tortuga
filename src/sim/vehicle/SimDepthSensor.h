#ifndef _RAM_SIM_SIMDEPTHSENSOR_H
#define _RAM_SIM_SIMDEPTHSENSOR_H

#include <ram.h>

#include "SimDevice.h"

namespace ram {
    namespace sim {
        class SimDepthSensor : virtual public vehicle::IDepthSensor, public SimDevice
        {
        private:
            btVector3 localPos;
            SimVehicle& parent;
            btScalar depth = 0;
        public:
            SimDepthSensor(SimVehicle& mParent, std::string mName) : SimDevice(mName), parent(mParent) {}
            
            virtual double getDepth(const ::Ice::Current&c)
            { return depth; }
            
            virtual void stepSimulation(SimWorld& world, btScalar timeStep)
            {
                depth = world.getGravity().dot(parent.getCenterOfMassPosition() + btTransform(parent.getOrientation()) * localPos) * world.getFluidDensity();
            }
        };
    }
}

#endif

#ifndef _RAM_SIM_SIMDEPTHSENSOR_H
#define _RAM_SIM_SIMDEPTHSENSOR_H

#include <ram.h>

#include "SimVehicle.h"
#include "SimDevice.h"

namespace ram {
    namespace sim {
        class SimDepthSensor : virtual public vehicle::DepthSensor, public SimDevice
        {
        private:
            btVector3 localPos;
            SimVehicle& parent;
            btScalar depth;
        public:
            SimDepthSensor(SimVehicle& mParent, std::string mName) : SimDevice(mName), parent(mParent), depth(0), localPos(0, 0, 0) {}
            
            virtual double getDepth(const ::Ice::Current&c)
            { return depth; }
            
            virtual void stepSimulation(SimWorld& world, btScalar timeStep)
            {
                depth = world.getFluidLevel() + (parent.getCenterOfMassTransform() * localPos).dot(world.getGravity().normalized());
            }
        };
    }
}

#endif

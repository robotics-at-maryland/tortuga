#ifndef _RAM_SIM_SIMTHRUSTER_H
#define _RAM_SIM_SIMTHRUSTER_H

#include <ram.h>

#include "SimVehicle.h"
#include "SimDevice.h"

namespace ram {
    namespace sim {
        class SimThruster : virtual public vehicle::IDepthSensor, public SimDevice
        {
        private:
            btTransform localTransform;
            SimVehicle& parent;
        public:
            SimThruster(SimVehicle& mParent, std::string mName) : SimDevice(mName), parent(mParent), localTransform() {}
            
            virtual void setForce(::Ice::Double, const ::Ice::Current& = ::Ice::Current())
            {
            }
            
            virtual ::Ice::Double getForce(const ::Ice::Current& = ::Ice::Current())
            {
            }
            
            virtual ::Ice::Double getMaxForce(const ::Ice::Current& = ::Ice::Current())
            {
            }
            
            virtual ::Ice::Double getMinForce(const ::Ice::Current& = ::Ice::Current())
            {
            }
            
            virtual ::Ice::Double getOffset(const ::Ice::Current& = ::Ice::Current())
            {
            }
            
            virtual bool isEnabled(const ::Ice::Current& = ::Ice::Current())
            {
            }
            
            virtual void setEnabled(bool, const ::Ice::Current& = ::Ice::Current())
            {
            }
            
            virtual void stepSimulation(SimWorld& world, btScalar timeStep)
            {
                depth = world.getFluidLevel() + (parent.getCenterOfMassTransform() * localPos).dot(world.getGravity().normalized());
            }
        };
    }
}

#endif

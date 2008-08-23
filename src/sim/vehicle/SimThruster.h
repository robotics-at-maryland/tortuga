#ifndef _RAM_SIM_SIMTHRUSTER_H
#define _RAM_SIM_SIMTHRUSTER_H

#include <ram.h>

#include "SimVehicle.h"
#include "SimDevice.h"

namespace ram {
    namespace sim {
        class SimThruster : virtual public vehicle::IThruster, public SimDevice, public BuoyantBody
        {
        private:
            btTransform localTransform;
            btDouble force;
            bool enabled;
            SimVehicle& parent;
        public:
            SimThruster(SimVehicle& mParent, std::string mName) : SimDevice(mName), parent(mParent), localTransform() {}
            
            virtual void setForce(::Ice::Double mForce, const ::Ice::Current&c = ::Ice::Current())
            {
                if (mForce < getMinForce())
                    force = getMinForce();
                else if (mForce > getMaxForce())
                    force = getMaxForce();
                else
                    force = mForce;
            }
            
            virtual ::Ice::Double getForce(const ::Ice::Current& = ::Ice::Current())
            {
                return force;
            }
            
            virtual ::Ice::Double getMaxForce(const ::Ice::Current& = ::Ice::Current())
            {
                return 100; /* TODO */
            }
            
            virtual ::Ice::Double getMinForce(const ::Ice::Current& = ::Ice::Current())
            {
                return -100; /* TODO */
            }
            
            virtual ::Ice::Double getOffset(const ::Ice::Current& = ::Ice::Current())
            {
                return 0;
            }
            
            virtual bool isEnabled(const ::Ice::Current& = ::Ice::Current())
            {
                return enabled;
            }
            
            virtual void setEnabled(bool state, const ::Ice::Current& = ::Ice::Current())
            {
                enabled = state;
            }
            
            virtual void stepSimulation(SimWorld& world, btScalar timeStep)
            {
                /* TODO */
            }
        };
    }
}

#endif

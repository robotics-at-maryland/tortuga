#ifndef _RAM_SIM_SIMPOWERSOURCE_H
#define _RAM_SIM_SIMPOWERSOURCE_H

#include <ram.h>
#include "SimDevice.h"

namespace ram {
    namespace sim {
        class SimPowerSource : virtual public vehicle::IPowerSource, public SimDevice
        {
        public:
            SimPowerSource(std::string mName) : SimDevice(mName) {}
            
            virtual bool isEnabled(const ::Ice::Current&c)
            { return false; /* TODO */ }
            
            virtual bool inUse(const ::Ice::Current&c)
            { return false; /* TODO */ }
            
            virtual void setEnabled(bool state, const ::Ice::Current&c)
            { /* TODO */ }
            
            virtual double getVoltage(const ::Ice::Current&c)
            { return 0; /* TODO */ }
            
            virtual double getCurrent(const ::Ice::Current&c)
            { return 0; /* TODO */ }
            
            virtual void stepSimulation(SimWorld& world, btScalar timeStep)
            { /* TODO */ }
        };
    }
}

#endif

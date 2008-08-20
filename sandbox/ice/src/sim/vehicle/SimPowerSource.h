#ifndef _RAM_SIM_SIMPOWERSOURCE_H
#define _RAM_SIM_SIMPOWERSOURCE_H

#include <ram.h>

namespace ram {
    namespace sim {
        class SimPowerSource : virtual public vehicle::IPowerSource
        {
        private:
            std::string name;
        public:
            SimPowerSource(std::string mName) : name(mName) {}
            
            virtual std::string getName(const ::Ice::Current&c)
            { return name; }
            
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
        };
    }
}

#endif

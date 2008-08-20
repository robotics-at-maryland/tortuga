#ifndef _RAM_MOCK_MOCKPOWERSOURCE_H
#define _RAM_MOCK_MOCKPOWERSOURCE_H

#include <ram.h>
#include "MockUtil.h"

namespace ram {
    namespace mock {
        class MockPowerSource : virtual public vehicle::IPowerSource
        {
        private:
            bool enabled;
            std::string name;
        public:
            MockPowerSource(std::string mName) : name(mName), enabled(true) {}
            
            virtual std::string getName(const ::Ice::Current&c)
            { return name; }
            
            virtual bool isEnabled(const ::Ice::Current&c)
            { return enabled; }
            
            virtual bool inUse(const ::Ice::Current&c)
            { return isEnabled(c); }
            
            virtual void setEnabled(bool state, const ::Ice::Current&c)
            { enabled = state; }
            
            virtual double getVoltage(const ::Ice::Current&c)
            { return inUse(c) * randUpTo(24); }
            
            virtual double getCurrent(const ::Ice::Current&c)
            { return inUse(c) * randUpTo(3); }
        };
    }
}

#endif

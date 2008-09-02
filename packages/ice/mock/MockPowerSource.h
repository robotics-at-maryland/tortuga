#ifndef _RAM_MOCK_MOCKPOWERSOURCE_H
#define _RAM_MOCK_MOCKPOWERSOURCE_H

#include <ram_ice.h>
#include "MockUtil.h"
#include "MockDevice.h"

namespace ram {
    namespace mock {
        class MockPowerSource : virtual public vehicle::PowerSource, public MockDevice
        {
        private:
            bool enabled;
        public:
            MockPowerSource(const std::string& name) : MockDevice(name), enabled(true) {}
            
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

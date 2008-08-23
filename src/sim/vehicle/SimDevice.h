#ifndef _RAM_SIM_SIMDEVICE_H
#define _RAM_SIM_SIMDEVICE_H

#include <ram.h>
#include "../SimWorld.h"

namespace ram {
    namespace sim {
        class SimDevice : virtual public vehicle::IDevice, public PhysicsUpdatable
        {
        private:
            std::string name;
        public:
            SimDevice(std::string mName) : name(mName) {}
            
            virtual std::string getName(const ::Ice::Current&c = ::Ice::Current())
            { return name; }
        };
    }
}

#endif

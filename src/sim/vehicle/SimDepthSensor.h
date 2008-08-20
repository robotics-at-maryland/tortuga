#ifndef _RAM_SIM_SIMDEPTHSENSOR_H
#define _RAM_SIM_SIMDEPTHSENSOR_H

#include <ram.h>

namespace ram {
    namespace sim {
        class SimDepthSensor : virtual public vehicle::IDepthSensor
        {
        private:
            std::string name;
        public:
            SimDepthSensor(std::string mName) : name(mName) {}
            
            virtual std::string getName(const ::Ice::Current&c)
            { return name; }
            
            virtual double getDepth(const ::Ice::Current&c)
            { return 0; /* TODO */ }
        };
    }
}

#endif

#ifndef _RAM_MOCK_MOCKDEVICE_H
#define _RAM_MOCK_MOCKDEVICE_H

#include <ram_ice.h>

namespace ram {
    namespace mock {
        class MockDevice : virtual public vehicle::Device
        {
        private:
            std::string name;
        public:
            MockDevice(const std::string& mName) : name(mName) {}
            
            virtual std::string getName(const ::Ice::Current&c)
            { return name; }
        };
    }
}

#endif

#include <ram.h>
#include "MockVehicle.h"

#ifndef _RAM_MOCK_MOCKVEHICLEFACTORY_H
#define _RAM_MOCK_MOCKVEHICLEFACTORY_H

namespace ram {
    namespace mock {
        class MockVehicleFactory : virtual public vehicle::VehicleFactory
        {
        private:
            vehicle::VehicleDictionary vehicles;
        public:
            MockVehicleFactory()
            {
                std::cerr << "Creating MockVehicleFactory." << std::endl;
            }
            ~MockVehicleFactory()
            {
                std::cerr << "Destroying MockVehicleFactory." << std::endl;
            }
            inline virtual vehicle::IVehiclePrx getVehicleByName(const ::std::string& name, const ::Ice::Current& c = ::Ice::Current())
            {
                // If the named vehicle does not exist, create it and add it
                // to the dictionary
                if (vehicles.find(name) == vehicles.end())
                {
                    std::cerr << "The vehicle '" << name << "' does not exist.  Creating it now." << std::endl;
                    vehicles[name] = vehicle::IVehiclePrx::uncheckedCast(c.adapter->addWithUUID(new MockVehicle(c)));
                }
                std::cerr << "Retrieving vehicle '" << name << "'." << std::endl;
                return vehicles[name];
            }
        };
    }
}

#endif

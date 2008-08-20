#ifndef _RAM_SIM_SIMVEHICLEFACTORY_H
#define _RAM_SIM_SIMVEHICLEFACTORY_H

#include <ram.h>
#include <Ice/Ice.h>
#include "SimVehicle.h"

namespace ram {
    namespace sim {
        class SimVehicleFactory : virtual public vehicle::IVehicleFactory
        {
        private:
            vehicle::VehicleDictionary vehicles;
        public:
            SimVehicleFactory()
            {
                std::cerr << "Creating MockVehicleFactory." << std::endl;
            }
            ~SimVehicleFactory()
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
                    vehicles[name] = vehicle::IVehiclePrx::uncheckedCast(c.adapter->addWithUUID(new SimVehicle(c)));
                }
                std::cerr << "Retrieving vehicle '" << name << "'." << std::endl;
                return vehicles[name];
            }
        };
    }
}

#endif

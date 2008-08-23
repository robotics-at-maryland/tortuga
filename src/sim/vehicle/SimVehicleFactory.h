#ifndef _RAM_SIM_SIMVEHICLEFACTORY_H
#define _RAM_SIM_SIMVEHICLEFACTORY_H

#include <ram.h>
#include "SimVehicle.h"
#include "../SimWorld.h"

#include <Ice/Ice.h>

namespace ram {
    namespace sim {
        class SimVehicleFactory : virtual public vehicle::IVehicleFactory {
        private:
            vehicle::VehicleDictionary vehicles;
            SimWorld& dynamicsWorld;
        public:
            SimVehicleFactory(SimWorld& world) : dynamicsWorld(world) {
                std::cerr << "Creating MockVehicleFactory." << std::endl;
            }
            ~SimVehicleFactory() {
                std::cerr << "Destroying MockVehicleFactory." << std::endl;
            }
            inline virtual vehicle::IVehiclePrx getVehicleByName(const ::std::string& name, const ::Ice::Current& c = ::Ice::Current())
            {
                // If the named vehicle does not exist, create it and add it
                // to the dictionary
                if (vehicles.find(name) == vehicles.end())
                {
                    std::cerr << "The vehicle '" << name << "' does not exist.  Creating it now." << std::endl;
                    SimVehicle* newVehicle = new SimVehicle(c);
                    vehicles[name] = vehicle::IVehiclePrx::uncheckedCast(c.adapter->addWithUUID(newVehicle));
                    dynamicsWorld.addRigidBody(newVehicle);
                }
                std::cerr << "Retrieving vehicle '" << name << "'." << std::endl;
                return vehicles[name];
            }
        };
    }
}

#endif

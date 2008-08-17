/*
 *  VehicleFactory.cpp
 *  ice
 *
 *  Created by Lady 3Jane Marie-France Tessier-Ashpool on 7/24/08.
 *  Copyright 2008 TRX Systems. All rights reserved.
 *
 */

#include <ram/sim/vehicle/VehicleFactory.h>
#include <ram/sim/vehicle/Vehicle.h>

#include <Ice/Ice.h>
#include <Ice/LocalException.h>

using namespace ram::sim::vehicle;
using namespace Ice;

::ram::vehicle::IVehiclePrx VehicleFactory::getVehicleByName(const ::std::string& name, const ::Ice::Current &c)
{
    try {
        Ice::CommunicatorPtr comm = c.adapter->getCommunicator();
	Vehicle* servant = new Vehicle(name);
        return ::ram::vehicle::IVehiclePrx::uncheckedCast(c.adapter->add(servant, comm->stringToIdentity(name)));
    } catch (const Ice::AlreadyRegisteredException& ex) {
        throw ::ram::vehicle::CannotCreateException(name);
    }
}

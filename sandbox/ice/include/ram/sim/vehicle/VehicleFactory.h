/*
 *  VehicleFactory.h
 *  ice
 *
 *  Created by Lady 3Jane Marie-France Tessier-Ashpool on 7/24/08.
 *  Copyright 2008 TRX Systems. All rights reserved.
 *
 */

#include <ram.h>

#ifndef _RAM_SIM_VEHICLEFACTORY_H
#define _RAM_SIM_VEHICLEFACTORY_H

namespace ram {
    namespace sim {
        namespace vehicle {
            class VehicleFactory : virtual public ::ram::vehicle::IVehicleFactory
            {
	    public:
	      boost::mutex
            public:
                virtual ::ram::vehicle::IVehiclePrx getVehicleByName(const ::std::string&, const ::Ice::Current& = ::Ice::Current());
            };
        }
    }
}

#endif

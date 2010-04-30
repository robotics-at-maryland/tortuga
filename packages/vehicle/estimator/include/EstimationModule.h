/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/include/EstimationModule.h
 */

/* EstimationModule provides the means for making hot-swappable estimation
 * algorithms.  This class does not specify the type of estimation that will
 * be done.  It is up to the user to intelligently select appropirate 
 * estimation modules and pass them the appropriate type of event.
*/

// Library Includes
#include <boost/shared_ptr.hpp>

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/Event.h"

namespace ram {
namespace estimator {

class EstimationModule;

typedef boost_shared_ptr<EstimationModule> EstimationModulePtr;

class EstimationModule 
{
public:

    /* The config node should contain the values necessary for initializing the
       specifig EstimationModule. */
    EstimationModule(){};
    virtual ~EstimationModule(){}

    /* update - the function that should be called to perform the estimation */
    virtual void update(core::EventPtr event) = 0;


private:
} 

} // namespace estimatior
} // namespace ram

/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Eliot Rudnick-Cohen <eliot@marlasrudnick.com>
 * All rights reserved.
 *
 * Author: Eliot Rudnick-Cohen <eliot@marlasrudnick.com>
 * File:  packages/estimation/src/modules/VisionEstimationModule.h
 */
#ifndef RAM_ESTIMATION_VISIONESTIMATIONMODULE_H
#define RAM_ESTIMATION_VISIONESTIMATIONMODULE_H

#include <boost/pointer_cast.hpp>
#include "estimation/include/EstimationModule.h"
#include "core/include/Events.h"
#include "math/include/Vector3.h"
namespace ram {
namespace estimation {

template<typename E>
class VisionEstimationModule: public EstimationModule
{
public:
    VisionEstimationModule<E>(core::Event::EventType type,
                              core::EventHubPtr eventHub,
                              core::ConfigNode config,
                              EstimatedStatePtr estState):
    EstimationModule(eventHub,"VisionEstimationModule",estState,type)
    {

        //will need to add name variable to the event class first(did this)
        //then use that to match up with name of obstacles
        //this will require making it possible to update obstacles in the first place
        //this will also require actually creating a system for handling the obstacles
        //in the first place.  To do this a map(see standard library container class map)
        //should be used to map the keys( the names) to the obstacles( the data), this is
        //in EstimatedState.
        //hopefully this is everything(it probably isn't)
        //also the obstacle class might require some modifications
    
        //ignore the above can just use templates.
        //also currently the plan is to subscribe to eventtypes in this class
    }

    void update(core::EventPtr event)
    {
        boost::shared_ptr<E> castEvent = boost::dynamic_pointer_cast<E>(event);

        // if the cast failed, bail out
        if(castEvent == 0)
            return;

        double dist = castEvent->range;
        dist = dist; // do nothing with this to avoid a warning

        math::Degree azimuth = castEvent->azimuth;
        math::Degree elevation = castEvent->elevation;

        // object position in the body frame
        math::Vector3 objectPos_b(0, 0, 0);

        // making the z value of the object the actual one for now
        
        // code to go from that to vector, the usual method(trig functions)
        
        math::Quaternion orientation = 
            m_estimatedState->getEstimatedOrientation();

        // express the vector from the camera to the object in the inertial
        // coordinate frame
        math::Vector3 objectPos_n = orientation.UnitInverse() * objectPos_b;

        // compute the vector from the origin of the inertial coordinate frame
        // to the object by adding the robot position in the inertial frame
        math::Vector2 robotPos = m_estimatedState->getEstimatedPosition();
        double robotDepth = m_estimatedState->getEstimatedDepth();

        math::Vector3 offset_n(robotPos[0], robotPos[1], robotDepth);
        objectPos_n += offset_n;

        // pretend the z value is now correctly assigned since it 
        // isn't stored yet
    }
};

} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_VISIONESTIMATIONMODULE_H

/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Eliot Rudnick-Cohen <eliot@marlasrudnick.com>
 * All rights reserved.
 *
 * Author: Eliot Rudnick-Cohen <eliot@marlasrudnick.com>
 * File:  packages/estimation/src/modules/VisionEstimationModule.h
 */
#ifndef RAM_ESTIMATION_VISIONESTIMATIONMODULE
#define RAM_ESTIMATION_VISIONESTIMATIONMODULE

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
    VisionEstimationModule<E>(core::Event::EventType type, core::EventHubPtr eventHub,
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
        math::Vector3 objectPos(0,0,0);
        //rest of the code
        
        boost::shared_ptr< E> castEvent =boost::dynamic_pointer_cast< E >(event);
        double dist=castEvent->range;
        dist=dist;
        math::Degree azimuth=castEvent->azimuth;
        math::Degree elevation=castEvent->elevation;
        //making the z value of the object the actual one for now
        objectPos[2]=0.0;
        
        //code to go from that to vector, the usual method(trig functions)
        
        math::Quaternion orientation=m_estimatedState->getEstimatedOrientation();
        objectPos=orientation.UnitInverse()*objectPos;
        objectPos[0]=(m_estimatedState->getEstimatedPosition())[0]+objectPos[0];
        objectPos[1]=(m_estimatedState->getEstimatedPosition())[1]+objectPos[1];
        //pretend the z value is now correctly assigned since it isn't stored yet


    }

private:
   





};



}//estimation namespace

}//ram namespace
#endif

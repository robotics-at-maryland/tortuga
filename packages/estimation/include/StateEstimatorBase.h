/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/include/StateEstimatorBase.h
 */

/* StateEstimatorBase is a basic implementation of an IStateEstimator to simpify
** the creation of more sophisticated state estimators.
*/

#ifndef RAM_ESTIMATION_STATEESTIMATORBASE_H
#define RAM_ESTIMATION_STATEESTIMATORBASE_H

// STD Includes

// Library Includes

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/Forward.h"
#include "core/include/EventHub.h"
#include "core/include/Updatable.h"

#include "estimation/include/IStateEstimator.h"
#include "estimation/include/EstimatedState.h"
#include "estimation/include/Obstacle.h"

#include "vehicle/include/Common.h"

#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"

namespace ram {
namespace estimation {

class StateEstimatorBase : public IStateEstimator
{
public:


    StateEstimatorBase(core::ConfigNode config,
                       core::EventHubPtr eventHub = core::EventHubPtr());

    StateEstimatorBase(core::ConfigNode config,
                       core::SubsystemList deps = core::SubsystemList());

    ~StateEstimatorBase(){};
 
    /* Define basic implementations of the functions required by IStateEstimator */

    /** Returns the estimated position */
    virtual math::Vector2 getEstimatedPosition();
    /** Returns the estimated velocity */
    virtual math::Vector2 getEstimatedVelocity();
    /** Returns the estimated linear acceleration */
    virtual math::Vector3 getEstimatedLinearAcceleration();
    /** Returns the estimated angular rate */
    virtual math::Vector3 getEstimatedAngularRate();
    /** Returns the estimated orientation */
    virtual math::Quaternion getEstimatedOrientation();
    /** Returns the estimated depth */
    virtual double getEstimatedDepth();
    /** Returns the estimated depth change rate */
    virtual double getEstimatedDepthDot();

    /** Adds an obstacle to the list of course obstacles for tracking
     *
     * @param name
     *    The name for the obstacle.
     *
     * @param obstacle
     *    A smart pointer to the obstacle that is to be added.
     */
    virtual void addObstacle(std::string name, ObstaclePtr obstacle);

    /** Returns the position of an obstacle
     *
     * @param name
     *    The name of the obstacle for which to retrieve the position.
     *
     * @return 
     *    A Vector2 object representing the position of the obstacle.
     */
    virtual math::Vector2 getObstaclePosition(std::string name);

    /** Returns the depth of an obstacle
     *
     * @param name
     *    The name of the obstacle for which to retrieve the depth.
     *
     * @return 
     *    A double representing the depth of the obstacle.
     */
    virtual double getObstacleDepth(std::string name);


    // Does nothing for now as the state estimator is event driven
    virtual void update(double timestep) {
    }
    
    virtual void setPriority(core::IUpdatable::Priority priority) {
    }
    
    virtual core::IUpdatable::Priority getPriority() {
        return core::IUpdatable::NORMAL_PRIORITY;
    }

    virtual void setAffinity(size_t affinity) {
    }
    
    virtual int getAffinity() {
        return core::IUpdatable::NORMAL_PRIORITY;
    }
    
    virtual void background(int interval) {
    }
    
    virtual void unbackground(bool join = false) {
    }

    virtual bool backgrounded() {
        return true;
    }

protected:

    /* These functions should be bound to events of the correct type.
    ** Each sensor will publish RAW_UPDATE events when new data is
    ** received.  Vision detectors will publish similar events when
    ** they compute the results from single frames.
    */
    virtual void rawUpdate_DVL(core::EventPtr event) = 0;
    virtual void rawUpdate_IMU(core::EventPtr event) = 0;
    virtual void rawUpdate_DepthSensor(core::EventPtr event) = 0;
    virtual void update_Vision(core::EventPtr event) = 0;
    virtual void update_Sonar(core::EventPtr event) = 0;

    /* These keep track of the event subscriptions.
    ** This class will delete them on deconstruction.
    */
    core::EventConnectionPtr updateConnection_IMU;
    core::EventConnectionPtr updateConnection_DVL;
    core::EventConnectionPtr updateConnection_DepthSensor;
    core::EventConnectionPtr updateConnection_Sonar;
    core::EventConnectionPtr updateConnection_Vision;

    // Pointer to access the estimated state for this estimator
    EstimatedStatePtr estimatedState;
};


} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_STATEESTIMATORBASE_H

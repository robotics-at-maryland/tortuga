/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/IController.h
 */

#ifndef RAM_CONTROL_ICONTROLLER_07_03_2007
#define RAM_CONTROL_ICONTROLLER_07_03_2007

// STD Includes
#include <string>

// Library Includes
#include <boost/shared_ptr.hpp>

// Project Includes
#include "core/include/Event.h"
#include "core/include/Subsystem.h"

#include "math/include/Quaternion.h"

#include "control/include/IDepthController.h"
#include "control/include/ITranslationalController.h"

// Must Be Included last
#include "control/include/Export.h"

namespace ram {
namespace control {

class IController;
typedef boost::shared_ptr<IController> IControllerPtr;
    
class RAM_EXPORT IController : public core::Subsystem,
                               public IDepthController,
                               public ITranslationalController
{
public:
    /**
     * \defgroup Events IController Events
     */
    /* @{ */
    
    /** When the controller desired depth changes
     *
     *  Type is ram::math::NumbericEvent;
     */
    static const core::Event::EventType DESIRED_DEPTH_UPDATE;

    /** When the desired orientation changes (ram::math::OrientationEvent) */
    static const core::Event::EventType DESIRED_ORIENTATION_UPDATE;

    /** When the vehicle reaches the depth set by the controller
     *
     *  Type is ram::math::NumbericEvent;
     */
    static const core::Event::EventType AT_DEPTH;

    /** When the vehicle reaches the orientation set by the controller
     *
     *  Type is ram::math::OrientationEvent
     */
    static const core::Event::EventType AT_ORIENTATION;

    /* @{ */

    /** Yaws the desired vehicle state by the desired number of degrees */
    virtual void yawVehicle(double degrees) = 0;

    /** Pitches the desired vehicle state by the desired number of degrees */
    virtual void pitchVehicle(double degrees) = 0;

    /** Rolls the desired vehicle state by the desired number of degrees */
    virtual void rollVehicle(double degrees) = 0;

    /** Gets the current desired orientation */
    virtual math::Quaternion getDesiredOrientation() = 0;
    
    /** Return to the home orientation */
    virtual inline void setHomeOrientation() { setDesiredOrientation(math::Quaternion::IDENTITY); }
	
    /** Sets the current desired orientation */
    virtual void setDesiredOrientation(math::Quaternion) = 0;
    
    /** Returns true if the vehicle is at the desired orientation */
    virtual bool atOrientation() = 0;

    /** Loads current orientation into desired (fixes offset in roll and pitch) */
    virtual void holdCurrentHeading() = 0;
    
    virtual void setBuoyantTorqueCorrection(double x, double y, double z) =0;
    
protected:
    IController(std::string name,
                core::EventHubPtr eventHub = core::EventHubPtr());
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_ICONTROLLER_07_03_2007

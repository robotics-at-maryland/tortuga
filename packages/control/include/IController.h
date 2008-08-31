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
#include "control/include/IRotationalController.h"

// Must Be Included last
#include "control/include/Export.h"

namespace ram {
namespace control {

class IController;
typedef boost::shared_ptr<IController> IControllerPtr;

/** A single interface from which to control a vehicle
 *
 *  This is a facade over a set of controllers which handle difference parts
 *  of the vehicles motion.
 */
class RAM_EXPORT IController : public core::Subsystem,
                               public IDepthController,
                               public ITranslationalController,
                               public IRotationalController
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
    
protected:
    IController(std::string name,
                core::EventHubPtr eventHub = core::EventHubPtr());
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_ICONTROLLER_07_03_2007

/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/IDepthController.h
 */

#ifndef RAM_CONTROL_IDEPTHCONTROLLER_08_07_2008
#define RAM_CONTROL_IDEPTHCONTROLLER_08_07_2008

// Library Includes
#include <boost/shared_ptr.hpp>

// Must Be Included last
#include "control/include/Export.h"

namespace ram {
namespace control {

class IDepthController;
typedef boost::shared_ptr<IDepthController> IDepthControllerPtr;

/** Defines the interface for a depth controller */
class RAM_EXPORT IDepthController
{
public:
    virtual ~IDepthController() {}

    /** Sets the desired depth of the sub in meters */
    virtual void setDepth(double depth) = 0;

    /** Current desired depth of the sub in meters */
    virtual double getDepth() = 0;
    
    /** Grab current estimated depth*/
    virtual double getEstimatedDepth() = 0;
    
    /** Grab current estimated depth velocity (depthDot)*/
    virtual double getEstimatedDepthDot() = 0;
    
    /** Returns true if the vehicle is at the desired depth */
    virtual bool atDepth() = 0;
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_IDEPTHCONTROLLER_08_07_2008

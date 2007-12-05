/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/Controller.h
 */

#ifndef RAM_CONTROL_ICONTROL_07_03_2007
#define RAM_CONTROL_ICONTROL_07_03_2007

// STD Includes
#include <string>

// Project Includes
#include "core/include/Subsystem.h"

// Must Be Included last
#include "control/include/Export.h"

namespace ram {
namespace control {

class RAM_EXPORT IController : public core::Subsystem
{
public:
    
    /** Set the current speed, clamped between -5 and 5 */
    virtual void setSpeed(int speed) = 0;

    /** Sets the current heading in degrees off north */
//    virtual void setHeading(double degrees) = 0;

    /** Sets the desired depth of the sub in meters */
    virtual void setDepth(double depth) = 0;

    /** Gets the current speed, a value between -5 and 5 */
    virtual int getSpeed() = 0;

    /** The current heading of th*/
//    virtual double getHeading() = 0;

    /** Current desired depth of the sub in meters */
    virtual double getDepth() = 0;

    /** Yaws the desired vehicle state by the desired number of degrees */
    virtual void yawVehicle(double degrees) = 0;

    /** Returns true if the vehicle is at the desired orientation */
    virtual bool isOriented() = 0;

    /** Returns true if the vehicle is at the desired depth */
    virtual bool atDepth() = 0;
    
protected:
	IController(std::string name);
	
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_ICONTROL_07_03_2007

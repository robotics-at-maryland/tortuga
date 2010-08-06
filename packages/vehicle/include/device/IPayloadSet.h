/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/IPayloadSet.h
 */

#ifndef RAM_VEHICLE_DEVICE_IPAYLOADSET_03_21_2009
#define RAM_VEHICLE_DEVICE_IPAYLOADSET_03_21_2009

// STD Includesb
#include <string>

// Project Includes
#include "vehicle/include/device/IDevice.h"

// Must Be Included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
namespace device {

/** A set of objects attached to the vehicle that can be released
 *
 *  This includes things like markers, small objects dropped from the vehicle,
 *  or torpedos, small (non-harmful) objects launched from the vehicle.
 */
class RAM_EXPORT IPayloadSet : public IDevice // For getName
             // boost::noncopyable
{
public:
    /** Event sent when an object from the PayloadSet is released */
    static const core::Event::EventType OBJECT_RELEASED;
    
    virtual ~IPayloadSet();

    /** Release one of the PayloadSet's objects
     *
     *  This only does something if objectCount > 0, and in that case it will
     *  send off an OBJECT_RELEASED event.
     */
    virtual void releaseObject() = 0;

    /** The numbers of objects left in the payload set */
    virtual int objectCount() = 0;

    /** The maximum number of objects in the payload set */
    virtual int initialObjectCount() = 0;
    
protected:
    IPayloadSet(core::EventHubPtr eventHub = core::EventHubPtr(),
                std::string name = "UNNAMED");
};

} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_IPAYLOADSET_03_21_2009

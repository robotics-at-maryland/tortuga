/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/IMarkerDropper.h
 */

#ifndef RAM_VEHICLE_DEVICE_IMARKERDROPPER_07_21_2008
#define RAM_VEHICLE_DEVICE_IMARKERDROPPER_07_21_2008

// STD Includesb
#include <string>

// Project Includes
#include "vehicle/include/device/IDevice.h"

// Must Be Included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
namespace device {

/** Controls the release of small objects dropped from the vehicle's bottom */
class RAM_EXPORT IMarkerDropper : public IDevice // For getName
             // boost::noncopyable
{
public:
    /** Event sent when a marker is dropped */
    static const core::Event::EventType MARKER_DROPPED;
    
    virtual ~IMarkerDropper();

    /** Release a marker
     *
     *  This only does something if markersLeft > 0, and in that case it will
     *  send off a MARKER_DROPPED event.
     */
    virtual void dropMarker() = 0;

    /** The numbers of markers left on the vehicle */
    virtual int markersLeft() = 0;

    /** The maximum number of markers on the vehicle */
    virtual int initalMarkerCount() = 0;
    
protected:
    IMarkerDropper(core::EventHubPtr eventHub = core::EventHubPtr());  
};

} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_IMARKERDROPPER_07_21_2008

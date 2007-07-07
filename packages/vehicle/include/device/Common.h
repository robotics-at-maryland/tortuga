/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/Common.h
 */

#ifndef RAM_VEHICLE_DEVICE_COMMON_06_11_2007
#define RAM_VEHICLE_DEVICE_COMMON_06_11_2007

// STD Includes
#include <vector>
#include <string>

// Library Includes
#include <boost/shared_ptr.hpp>

namespace ram {
namespace vehicle {
namespace device {
    
// Forward Declartions
class IDevice;
typedef boost::shared_ptr<IDevice> IDevicePtr;

class Device;
typedef boost::shared_ptr<Device> DevicePtr;

class Thruster;
typedef boost::shared_ptr<Thruster> ThrusterPtr;

class ThrusterCommand;
typedef boost::shared_ptr<ThrusterCommand> ThrusterCommandPtr;

class IMU;
typedef boost::shared_ptr<IMU> IMUPtr;

class PSU;
typedef boost::shared_ptr<PSU> PSUPtr;

typedef std::vector<double> DoubleList;
typedef DoubleList VoltageList;
typedef DoubleList WattageList;
typedef DoubleList CurrentList;
typedef std::vector<std::string> StringList;
    
namespace details {
/** To expose things to GCC-XML */
inline int instantiate()
{
    int a = 0;
    a += sizeof(boost::shared_ptr<IDevice>);
    a += sizeof(boost::shared_ptr<IDevice>);
    a += sizeof(boost::shared_ptr<Thruster>);
    a += sizeof(boost::shared_ptr<ThrusterCommand>);
    a += sizeof(boost::shared_ptr<IMU>);
    a += sizeof(boost::shared_ptr<PSU>);
    a += sizeof(DoubleList);
    a += sizeof(VoltageList);
    a += sizeof(WattageList);
    a += sizeof(CurrentList);
    a += sizeof(StringList);
    return a;
}
}
    
} // namespace device
} // namespace vehicle
} // namespace ram



#endif // RAM_VEHICLE_DEVICE_COMMON_06_11_2007

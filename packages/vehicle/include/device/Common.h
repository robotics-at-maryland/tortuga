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

class IThruster;
typedef boost::shared_ptr<IThruster> IThrusterPtr;
    
class IPowerSource;
typedef boost::shared_ptr<IPowerSource> IPowerSourcePtr;

class ITempSensor;
typedef boost::shared_ptr<ITempSensor> ITempSensorPtr;

class ISonar;
typedef boost::shared_ptr<ISonar> ISonarPtr;

class IIMU;
typedef boost::shared_ptr<IIMU> IIMUPtr;
    
class IPSU;
typedef boost::shared_ptr<IPSU> IPSUPtr;

class IDepthSensor;
typedef boost::shared_ptr<IDepthSensor> IDepthSensorPtr;

class IPayloadSet;
typedef boost::shared_ptr<IPayloadSet> IPayloadSetPtr;
    
class SensorBoard;
typedef boost::shared_ptr<SensorBoard> SensorBoardPtr;
    

typedef std::vector<double> DoubleList;
typedef DoubleList VoltageList;
typedef DoubleList WattageList;
typedef DoubleList CurrentList;
typedef std::vector<std::string> StringList;
    
} // namespace device
} // namespace vehicle
} // namespace ram


// Magic namespace to clean up names
namespace pyplusplus { 
namespace aliases {

typedef ram::vehicle::device::IDevicePtr IDevicePtr;
typedef ram::vehicle::device::IThrusterPtr IThrusterPtr;
typedef ram::vehicle::device::IPowerSourcePtr IPowerSourcePtr;
typedef ram::vehicle::device::ITempSensorPtr ITempSensorPtr;

}
}

// To expose things to GCC-XML
namespace details {
inline int instantiateVehicleDeviceCommon()
{
    int a = sizeof(boost::shared_ptr<ram::vehicle::device::IDevice>);
    a += sizeof(boost::shared_ptr<ram::vehicle::device::IThruster>);
    a += sizeof(boost::shared_ptr<ram::vehicle::device::IPowerSource>);
    a += sizeof(boost::shared_ptr<ram::vehicle::device::ITempSensor>);
/*    a += sizeof(DoubleList);
    a += sizeof(VoltageList);
    a += sizeof(WattageList);
    a += sizeof(CurrentList);
    a += sizeof(StringList);*/
    return a;
}
}


#endif // RAM_VEHICLE_DEVICE_COMMON_06_11_2007

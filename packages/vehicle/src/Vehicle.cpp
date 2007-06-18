#include "Vehicle.h"
#include "device/PSU.h"

namespace vehicle {

Vehicle::Vehicle()
{
    device::Device* dev = new device::PSU();
    m_devices[dev->getName()] = dev;
}

device::Device* Vehicle::getDevices(std::string name)
{
    return m_devices[name];
}

}

#include "Vehicle.h"
#include "PSU.h"

Vehicle::Vehicle()
{
    Device* dev = new PSU();
    m_devices[dev->getName()] = dev;
}

Device* Vehicle::getDevices(std::string name)
{
    return m_devices[name];
}

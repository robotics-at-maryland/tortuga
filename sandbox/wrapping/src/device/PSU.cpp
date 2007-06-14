#include "device/PSU.h"

namespace vehicle {
namespace device {

std::string PSU::getName()
{
    return "PSU";
}

double PSU::getVoltage()
{
    return 8.73;
}

}
}

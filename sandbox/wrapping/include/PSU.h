#ifndef TEST_PSU_H
#define TEST_PSU_H

#include "Device.h"

class PSU : public Device
{
public:
    virtual std::string getName();

    double getVoltage();
};

#endif // TEST_PSU_H

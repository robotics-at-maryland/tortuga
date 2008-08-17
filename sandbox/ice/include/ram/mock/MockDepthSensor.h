#ifndef _RAM_MOCK_MOCKDEPTHSENSOR_H
#define _RAM_MOCK_MOCKDEPTHSENSOR_H

#include <ram.h>
#include <stdlib.h>
#include "MockUtil.h"

namespace ram {
    class MockIMU : virtual public vehicle::IIMU
    {
    public:
        virtual double getDepth(const ::Ice::Current&c)
        { return randDouble(25); }
    };
}

#endif

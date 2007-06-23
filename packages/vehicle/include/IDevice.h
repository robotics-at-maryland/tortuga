/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/IUpdatable.h
 */

#ifndef RAM_CORE_IUPDATABLE_06_22_2006
#define RAM_CORE_IUPDATABLE_06_22_2006

namespace ram {
namespace core {

class IDevice : public IUpdatable
{
public:
    virtual ~IDevice() {};

    virtual std::string get_name();
};

} // namespace vehicle
} // namespace ram

/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/EventConnection.h
 */

#ifndef RAM_CORE_EVENCONNECTION_H_11_30_2007
#define RAM_CORE_EVENCONNECTION_H_11_30_2007

// Library Includes
#include <boost/utility.hpp>

// Project Includes
#include "core/include/Event.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

/** Represents the connection between and event handler, and publisher
 * 
 *  You must call disconnect on this object before the handler is destroyed,
 *  or any further calls to the handler will result in crashes.
 */
class RAM_EXPORT EventConnection : boost::noncopyable
{
public:
    virtual ~EventConnection() {}
    virtual void disconnect() = 0;
protected:
    EventConnection() {}
};
    
} // namespace core
} // namespace ram
    
#endif // RAM_CORE_EVENCONNECTION_H_11_30_2007

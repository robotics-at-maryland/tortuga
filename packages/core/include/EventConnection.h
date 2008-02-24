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

/** Represents the connection between an event handler and a publisher
 * 
 *  You must call disconnect on this object before the handler is destroyed,
 *  or any further calls to the handler will result in a crash.
 */
class RAM_EXPORT EventConnection : boost::noncopyable
{
public:
    virtual ~EventConnection() {}

    /** Disconnects the events, the handler function will not be called */
    virtual void disconnect() = 0;
    
    /** Return true if disconnect has been called */
    virtual bool connected() = 0;
protected:
    EventConnection() {}
};
    
} // namespace core
} // namespace ram
    
#endif // RAM_CORE_EVENCONNECTION_H_11_30_2007

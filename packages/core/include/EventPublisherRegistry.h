/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/EventPublisherRegistry.h
 */

#ifndef RAM_CORE_EVENTPUBLISHERREGISTRY_H_06_01_2009
#define RAM_CORE_EVENTPUBLISHERREGISTRY_H_06_01_2009

// STD Includes
#include <map>
#include <string>

// Project Includes
#include "core/include/Forward.h"
#include "core/include/ReadWriteMutex.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

class RAM_EXPORT EventPublisherRegistry
{
public:
    /** Lookup an EventPublisher based on its name
     *
     *  A name is only given if you supply one to the constructor, if the
     *  default value of "UNNAMED" is used, it will no be recorded in the
     *  registry.
     *
     *  @return
     *      The pointer to the EventPublisher or 0 if the EventPublisher does
     *      not exist.
     */
    static EventPublisher* lookupByName(std::string name);
    
    /** Register the publisher in the registry based on its name */
    static void registerPublisher(EventPublisher* publisher);

    /** Remove the publishers from the registry under its name */
    static void unRegisterPublisher(EventPublisher* publisher);

private:
    /** Get the mutex used to share access to the registry */
    static ReadWriteMutex* getRegistryMutex();

    /** Get the pointer to the registry */
    static std::map<std::string, EventPublisher*>* getRegistry();
};

} // namespace core
} // namespace ram
    
#endif // RAM_CORE_EVENTPUBLISHERREGISTRY_H_06_01_2009

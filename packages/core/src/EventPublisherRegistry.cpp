/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/EventPublisherRegistry.cpp
 */

// STD Include
#include <utility>

// Project Includes
#include "core/include/EventPublisherRegistry.h"
#include "core/include/EventPublisher.h"

namespace ram {
namespace core {

EventPublisher* EventPublisherRegistry::lookupByName(std::string name)
{
    ReadWriteMutex::ScopedWriteLock lock(*getRegistryMutex());
    std::map<std::string, EventPublisher*>::iterator iter =
        getRegistry()->find(name);
    if (getRegistry()->end() != iter)
        return iter->second;
    else
        return 0;
}
    
void EventPublisherRegistry::registerPublisher(EventPublisher* publisher)
{
    std::string name(publisher->getPublisherName());
    
    ReadWriteMutex::ScopedWriteLock lock(*getRegistryMutex());

    // Insert into the registry.
    // NOTE: This does not check for overwrites, it was found that the tests,
    // particullarly the python ones, objects are not cleaned up in a order
    // which keeps the registry without duplicates
    getRegistry()->insert(std::make_pair(name, publisher));
}


void EventPublisherRegistry::unRegisterPublisher(EventPublisher* publisher)
{
    std::string name(publisher->getPublisherName());
    
    ReadWriteMutex::ScopedWriteLock lock(*getRegistryMutex());

    // Make sure its currently a member of the registry
    std::map<std::string, EventPublisher*>::iterator iter =
        getRegistry()->find(name);

    // Remove from the registry if its a memeber
    if(getRegistry()->end() != iter)
        getRegistry()->erase(iter);
}
    
ReadWriteMutex* EventPublisherRegistry::getRegistryMutex()
{
    static ReadWriteMutex mutex;
    return &mutex;
}

std::map<std::string, EventPublisher*>* EventPublisherRegistry::getRegistry()
{
    static std::map<std::string, EventPublisher*> registry;
    return &registry;
}


} // namespace core
} // namespace ram

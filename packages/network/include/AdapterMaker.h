/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/network/include/AdapterMaker.h
 */

#ifndef RAM_NETWORK_ADAPTER_MAKER_H_12_16_2010
#define RAM_NETWORK_ADAPTER_MAKER_H_12_16_2010

// STD Includes
#include <string>

// Library Includes
#include <Ice/Ice.h>

// Project Includes
#include "core/include/Subsystem.h"
#include "pattern/include/Maker.h"

namespace ram {
namespace network {

typedef core::SubsystemPtr AdapterMakerParamType;

struct AdapterKeyExtractor
{
    static std::string extractKey(AdapterMakerParamType& params)
    {
        return params->getName();
    }
};

typedef ram::pattern::Maker<Ice::ObjectPtr,
                            AdapterMakerParamType,
                            std::string,
                            AdapterKeyExtractor> AdapterMaker;

template <class AdapterType>
struct AdapterMakerTemplate : public AdapterMaker
{
    AdapterMakerTemplate(std::string subsystemName) :
        AdapterMaker(subsystemName) {};

    virtual Ice::ObjectPtr makeObject(
        AdapterMakerParamType params)
    {
        return new AdapterType(params);
    }
};

} // namespace network
} // namespace ram

#define RAM_NETWORK_REGISTER_PROXY(TYPE, NAME)  \
    static ram::network::AdapterMakerTemplate< TYPE > register ## NAME(# NAME)

#endif // RAM_NETWORK_ADAPTER_MAKER_H_12_16_2010

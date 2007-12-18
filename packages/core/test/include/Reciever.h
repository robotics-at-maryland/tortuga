/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/include/Reciever.h
 */

#ifndef RAM_CORE_TEST_RECIEVER_12_17_2007
#define RAM_CORE_TEST_RECIEVER_12_17_2007

// STD Includes
#include <vector>

// Library Includes
#include "core/include/Event.h"

struct Reciever
{
    Reciever() : calls(0) {}

    void handler(ram::core::EventPtr event_)
     {
         calls++;
         events.push_back(event_);
     }
    
    int calls;
    std::vector<ram::core::EventPtr> events;
};

#endif // RAM_CORE_TEST_RECIEVER_12_17_2007

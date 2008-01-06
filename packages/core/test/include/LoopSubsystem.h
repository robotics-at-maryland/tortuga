/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/include/MockSubsystem.h
 */

#ifndef RAM_CORE_TEST_LOOPSUBSYSTEM_01_06_2008
#define RAM_CORE_TEST_LOOPSUBSYSTEM_01_06_2008

// STD Includes
#include <vector>

// Project Includes
#include "core/include/TimeVal.h"
#include "core/test/include/MockSubsystem.h"

const static long USEC_PER_MILLISEC = 1000;

class LoopSubsystem : public MockSubsystem
{
public:
    static const std::string STOP;
    
    LoopSubsystem(ram::core::ConfigNode config_, 
                  ram::core::SubsystemList dependents_) :
        MockSubsystem(config_, dependents_),
        m_start(0, 0),
        m_sleepTime(config_["sleepTime"].asInt()),
        m_stopIterations(config_["stopIterations"].asInt(-1))
    {}
    
    virtual void update(double timeSinceLastUpdate)
    {
        if (0 == m_stopIterations)
            publish(STOP, ram::core::EventPtr(new ram::core::Event()));
        m_stopIterations -= 1;
        
        // Don't record first update
        if (0 != m_start.get_double())
        {
            ram::core::TimeVal timePassed(ram::core::TimeVal::timeOfDay() -
                                          m_start);
            actualUpdates.push_back(timePassed.get_double());
            expectedUpdates.push_back(timeSinceLastUpdate);
        }
        
        m_start.now();
        
        usleep(USEC_PER_MILLISEC * m_sleepTime);
    }

    std::vector<double> actualUpdates;
    std::vector<double> expectedUpdates;
    
private:
    ram::core::TimeVal m_start;
    int m_sleepTime;
    int m_stopIterations;
};

#endif // RAM_CORE_TEST_LOOPSUBSYSTEM_01_06_2008

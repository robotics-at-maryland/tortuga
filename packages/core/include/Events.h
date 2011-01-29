/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/Events.h
 */

#ifndef RAM_CORE_EVENTS_06_04_2009
#define RAM_CORE_EVENTS_06_04_2009

// STD Includes
#include <vector>
#include <string>

// Project Includes
#include "core/include/Event.h"

namespace ram {
namespace core {

struct StringEvent : public core::Event
{
    std::string string;

    virtual EventPtr clone();
};

typedef boost::shared_ptr<StringEvent> StringEventPtr;

struct IntEvent : public core::Event
{
    IntEvent() : data(0) {}

    IntEvent(int data_) : data(data_) {}

    virtual EventPtr clone();

    int data;
};

typedef boost::shared_ptr<IntEvent> IntEventPtr;

struct Plot1DEvent : public core::Event
{
    Plot1DEvent() : m_plotName(""),
                    m_data(0),
                    m_labels(0) {}

    Plot1DEvent(std::string name,
                std::vector<double> data,
                std::vector<std::string> labels) : m_plotName(name),
                                              m_data(data),
                                              m_labels(labels) {}

    virtual EventPtr clone();

    std::string m_plotName;

    // each vector element corresponds to a data series
    std::vector<double> m_data;
    std::vector<std::string> m_labels;

};

typedef boost::shared_ptr<Plot1DEvent> Plot1DEventPtr;

struct Plot2DEvent : public core::Event
{
    Plot2DEvent() : m_plotName(""),
                    m_xData(0), m_yData(0),
                    m_xLabels(0), m_yLabels(0) {}

    Plot2DEvent(std::string name,
                std::vector<double> xData,
                std::vector<double> yData,
                std::vector<std::string> xLabels,
                std::vector<std::string> yLabels) : m_plotName(""),
                                               m_xData(xData),
                                               m_yData(yData),
                                               m_xLabels(xLabels),
                                               m_yLabels(yLabels) {}

        
    virtual EventPtr clone();

    std::string m_plotName;

    // each vector element corresponds to a data series
    std::vector<double> m_xData;
    std::vector<double> m_yData;
    std::vector<std::string> m_xLabels;
    std::vector<std::string> m_yLabels;
};

typedef boost::shared_ptr<Plot2DEvent> Plot2DEventPtr;

} // namespace core
} // namespace ram

#endif // RAM_CORE_EVENTS_06_04_2009

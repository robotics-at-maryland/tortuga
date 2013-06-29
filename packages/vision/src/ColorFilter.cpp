/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/ColorFilter.cpp
 */

// STD Includes 
#include <cstring>
#include <sstream>

// Library Includes
#include "boost/bind.hpp"
#include "cv.h" //kate changed from cxtypes.h to cv.h in an attempt to update openCV 6-6-2013
#include "highgui.h"

// Project Includes
#include "vision/include/ColorFilter.h"
#include "vision/include/Image.h"

#include "core/include/PropertySet.h"
#include "core/include/ConfigNode.h"

namespace ram {
namespace vision {

ColorFilter::ColorFilter(
    unsigned char channel1Low, unsigned char channel1High,
    unsigned char channel2Low, unsigned char channel2High,
    unsigned char channel3Low, unsigned char channel3High) :

    m_channel1Low(channel1Low),
    m_channel1High(channel1High),
    m_channel2Low(channel2Low),
    m_channel2High(channel2High),
    m_channel3Low(channel3Low),
    m_channel3High(channel3High)
{
    setupRanges();
}

ColorFilter::~ColorFilter()
{
}

void ColorFilter::setupRanges()
{
    // Zero the ranges
    memset(m_channel1Range, 0, sizeof(m_channel1Range));
    memset(m_channel2Range, 0, sizeof(m_channel2Range));
    memset(m_channel3Range, 0, sizeof(m_channel3Range));


    // pass from low to high
    if (m_channel1Low <= m_channel1High){
        memset(m_channel1Range + m_channel1Low, 255, 
               m_channel1High - m_channel1Low + 1);
    } else {
        // pass from 0 to high and low to 255
        if (m_channel1High < m_channel1Low){
            memset(m_channel1Range, 255, m_channel1High + 1);
            memset(m_channel1Range + m_channel1Low, 255,
                   255 - m_channel1Low);
        }
    }

    // pass from low to high
    if (m_channel2Low <= m_channel2High){
        memset(m_channel2Range + m_channel2Low, 255, 
               m_channel2High - m_channel2Low + 1);
    } else {
        // pass from 0 to high and low to 255
        if (m_channel2High < m_channel2Low){
            memset(m_channel2Range, 255, m_channel2High + 1);
            memset(m_channel2Range + m_channel2Low, 255,
                   255 - m_channel2Low);
        }
    }

    // pass from low to high
    if (m_channel3Low <= m_channel3High){
        memset(m_channel3Range + m_channel3Low, 255, 
               m_channel3High - m_channel3Low + 1);
    } else {
        // pass from 0 to high and low to 255
        if (m_channel3High < m_channel3Low){
            memset(m_channel3Range, 255, m_channel3High + 1);
            memset(m_channel3Range + m_channel3Low, 255,
                   255 - m_channel3Low);
        }
    }

}

void ColorFilter::filterImage(Image* input, Image* output)
{
    int numPixels = input->getWidth() * input->getHeight();
    int nChannels;
    unsigned char* inputData = input->getData();
    unsigned char* outputData = NULL;
    if (output)
    {
        outputData = output->getData();
        nChannels = output->getNumChannels();
    }
    else
    {
        outputData = input->getData();
        nChannels = input->getNumChannels();
    }

    for (int i = 0; i < numPixels; ++i)
    {
        unsigned char result = 
            m_channel1Range[*inputData] & 
            m_channel2Range[*(inputData + 1)] &
            m_channel3Range[*(inputData + 2)];
        
        for (int k = 0; k < nChannels; k++, outputData++)
            *outputData = result;

        inputData += 3;
    }
}

void ColorFilter::inverseFilterImage(Image* input, Image *output)
{
    int numPixels = input->getWidth() * input->getHeight();
    int nChannels;
    unsigned char* inputData = input->getData();
    unsigned char* outputData = 0;
    if (output) {
        outputData = output->getData();
        nChannels = output->getNumChannels();
    } else {
        outputData = input->getData();
        nChannels = input->getNumChannels();
    }

    for (int i = 0; i < numPixels; ++i)
    {
        unsigned char result = 255 * !(
            m_channel1Range[*inputData] & 
            m_channel2Range[*(inputData + 1)] &
            m_channel3Range[*(inputData + 2)]);
        
        for (int k = 0; k < nChannels; k++, outputData++) {
            *outputData = result;
        }
        
        inputData += 3;
    }
}




void ColorFilter::setChannel1Low(int value)
{
    // Ensure the value is in the proper range
    assert(value < 256 && "Range value overflow");

    // Store the new value
    m_channel1Low = (unsigned char)value;

    // Setup the new ranges
    setupRanges();
}

void ColorFilter::setChannel1High(int value)
{
    // Ensure the value is in the proper range
    assert(value < 256 && "Range value overflow");

    // Store the new value
    m_channel1High = (unsigned char)value;

    // Setup the new ranges
    setupRanges();
}

void ColorFilter::setChannel2Low(int value)
{
    // Ensure the value is in the proper range
    assert(value < 256 && "Range value overflow");

    // Store the new value
    m_channel2Low = (unsigned char)value;

    // Setup the new ranges
    setupRanges();
}

void ColorFilter::setChannel2High(int value)
{
    // Ensure the value is in the proper range
    assert(value < 256 && "Range value overflow");

    // Store the new value
    m_channel2High = (unsigned char)value;

    // Setup the new ranges
    setupRanges();
}

void ColorFilter::setChannel3Low(int value)
{
    // Ensure the value is in the proper range
    assert(value < 256 && "Range value overflow");

    // Store the new value
    m_channel3Low = (unsigned char)value;

    // Setup the new ranges
    setupRanges();
}

void ColorFilter::setChannel3High(int value)
{
    // Ensure the value is in the proper range
    assert(value < 256 && "Range value overflow");

    // Store the new value
    m_channel3High = (unsigned char)value;

    // Setup the new ranges
    setupRanges();
}

int ColorFilter::getChannel1Low()
{
    return m_channel1Low;
}

int ColorFilter::getChannel1High()
{      
    return m_channel1High;
}

int ColorFilter::getChannel2Low()
{
    return m_channel2Low;
}

int ColorFilter::getChannel2High()
{      
    return m_channel2High;
}

int ColorFilter::getChannel3Low()
{
    return m_channel3Low;
}

int ColorFilter::getChannel3High()
{      
    return m_channel3High;
}
    
void ColorFilter::addPropertiesToSet(
    core::PropertySetPtr propSet, core::ConfigNode* config,
    std::string channel1Name, std::string channel1Desc,
    std::string channel2Name, std::string channel2Desc,
    std::string channel3Name, std::string channel3Desc,
    int channel1LowDefault, int channel1HighDefault,
    int channel2LowDefault, int channel2HighDefault,
    int channel3LowDefault, int channel3HighDefault)
{
    propSet->addProperty(*config, false,
                         getShortChannelName(channel1Name, true),
                         getChannelDescription(channel1Desc, true), channel1LowDefault,
                         boost::bind(&ColorFilter::getChannel1Low, this),
                         boost::bind(&ColorFilter::setChannel1Low, this, _1), 0, 255);

    propSet->addProperty(*config, false,
                         getShortChannelName(channel1Name, false),
                         getChannelDescription(channel1Desc, false), channel1HighDefault,
                         boost::bind(&ColorFilter::getChannel1High, this),
                         boost::bind(&ColorFilter::setChannel1High, this, _1), 0, 255);
    
    propSet->addProperty(*config, false,
                         getShortChannelName(channel2Name, true),
                         getChannelDescription(channel2Desc, true), channel2LowDefault,
                         boost::bind(&ColorFilter::getChannel2Low, this),
                         boost::bind(&ColorFilter::setChannel2Low, this, _1), 0, 255);

    propSet->addProperty(*config, false,
                         getShortChannelName(channel2Name, false),
                         getChannelDescription(channel2Desc, false), channel2HighDefault,
                         boost::bind(&ColorFilter::getChannel2High, this),
                         boost::bind(&ColorFilter::setChannel2High, this, _1), 0, 255);

    propSet->addProperty(*config, false,
                         getShortChannelName(channel3Name, true),
                         getChannelDescription(channel3Desc, true), channel3LowDefault,
                         boost::bind(&ColorFilter::getChannel3Low, this),
                         boost::bind(&ColorFilter::setChannel3Low, this, _1), 0, 255);

    propSet->addProperty(*config, false,
                         getShortChannelName(channel3Name, false),
                         getChannelDescription(channel3Desc, false), channel3HighDefault,
                         boost::bind(&ColorFilter::getChannel3High, this),
                         boost::bind(&ColorFilter::setChannel3High, this, _1), 0, 255);

}


std::string ColorFilter::getShortChannelName(std::string shortName, bool isMin)
{
    std::stringstream ss;
    ss << "filt" << shortName;

    if (isMin)
        ss << "Min";
    else
        ss << "Max";

    return ss.str();
}
    
std::string ColorFilter::getChannelDescription(std::string descriptionName,
                                               bool isMin)
{
    std::stringstream ss;

    if (isMin)
        ss << "Min ";
    else
        ss << "Max ";

    ss << descriptionName << " value for a pixel of the desired color";
    
    return ss.str();
}
    
} // namespace vision
} // namespace ram

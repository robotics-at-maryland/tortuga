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

// Project Includes
#include "vision/include/ColorFilter.h"
#include "vision/include/Image.h"

namespace ram {
namespace vision {

ColorFilter::ColorFilter(unsigned char channel1Low, unsigned char channel1High,
                         unsigned char channel2Low, unsigned char channel2High,
                         unsigned char channel3Low, unsigned char channel3High)
{
    memset(m_channel1Range, sizeof(m_channel1Range), 0);
    memset(m_channel2Range, sizeof(m_channel2Range), 0);
    memset(m_channel3Range, sizeof(m_channel3Range), 0);

    for (int i = channel1Low; i < channel1High; ++i)
        m_channel1Range[i] = 255;
    for (int i = channel2Low; i < channel2High; ++i)
        m_channel2Range[i] = 255;
    for (int i = channel3Low; i < channel3High; ++i)
        m_channel3Range[i] = 255;
}

ColorFilter::~ColorFilter()
{
}

void ColorFilter::filterImage(Image* input)
{
    int numPixels = input->getWidth() * input->getHeight();
    unsigned char* data = input->getData();

    for (int i = 0; i < numPixels; ++i)
    {
        unsigned char result = 
	  m_channel1Range[*data] & 
	  m_channel2Range[*(data + 1)] &
	  m_channel3Range[*(data + 2)];

	*data = result;
	*(data + 1) = result;
	*(data + 2) = result;

	data += 3;
    }
}
    
} // namespace vision
} // namespace ram

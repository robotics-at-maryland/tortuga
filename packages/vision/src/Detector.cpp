/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/Detector.cpp
 */


// Project Includes
#include "vision/include/Detector.h"
#include "vision/include/Image.h"

#include "core/include/PropertySet.h"

namespace ram {
namespace vision {

Detector::Detector(core::EventHubPtr eventHub) :
    core::EventPublisher(eventHub),
    m_propertySet(new core::PropertySet())
{
}

core::PropertySetPtr Detector::getPropertySet()
{
    return m_propertySet;
}

void Detector::imageToAICoordinates(const Image* image, 
                                    const int& imageX, const int& imageY,
                                    double& outX, double& outY)
{
    // Shift origin to the center
    outX = -1 * ((image->getWidth() / 2.0) - (double)imageX);
    outY = (image->getHeight() / 2.0) - (double)imageY;
    
    // Normalize (-1 to 1)
    outX = outX / ((double)(image->getWidth())) * 2.0;
    outY = outY / ((double)(image->getHeight())) * 2.0;
    
    // Account for the aspect ratio difference
    // 640/480
    outX *= (double)image->getWidth() / image->getHeight();
}
  
} // namespace vision
} // namespace ram


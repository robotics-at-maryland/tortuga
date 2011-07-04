/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/Detector.cpp
 */

#include <iostream>

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
    double halfImgWidth = (1/2.0) * image->getWidth();
    double halfImgHeight = (1/2.0) * image->getHeight();

    // Shift origin to the center
    outX = (imageX - halfImgWidth) / halfImgWidth;
    outY = - (imageY - halfImgHeight) / halfImgHeight;;
}
  
} // namespace vision
} // namespace ram


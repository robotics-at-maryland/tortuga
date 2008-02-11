/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/Image.cpp
 */

// Librar Includes
#include "highgui.h"

// Project Includes
#include "vision/include/OpenCVImage.h"

namespace ram {
namespace vision {

Image* Image::loadFromFile(std::string fileName)
{
    IplImage* image = cvLoadImage(fileName.c_str());
    if (image)
        return new OpenCVImage(image, true);
    return 0;
}

void Image::saveToFile(Image* image, std::string fileName)
{
    cvSaveImage(fileName.c_str(), image->asIplImage());
}

Image* Image::loadFromBuffer(unsigned char* buffer, int width, int height,
                             bool ownership)
{
    return new OpenCVImage(buffer, width, height, ownership);
}

} // namespace vision
} // namespace ram

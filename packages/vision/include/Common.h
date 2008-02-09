/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/Common.h
 */

#ifndef RAM_VISION_COMMON_H_05_29_2007
#define RAM_VISION_COMMON_H_05_29_2007

// Library Includes
#include <boost/shared_ptr.hpp>

namespace ram {
namespace vision {

// Forware declaration of all classes
class Camera;
class Image;
class OpenCVImage;
class OpenCVCamera;
class Calibration;
    
class Detector;
typedef boost::shared_ptr<Detector> DetectorPtr;
    
} // namespace vision
} // namespace ram

// Forward declared to avoid OpenCV Header
struct _IplImage;
typedef struct _IplImage IplImage;

#endif // RAM_VISION_COMMON_H_05_29_2007

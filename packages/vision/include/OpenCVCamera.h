/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim and Joseph Lisee
 * File:  packages/vision/include/OpenCVCamera.h
 */

#ifndef RAM_VISION_OPENCVCAMERA_H_06_06_2007
#define RAM_VISION_OPENCVCAMERA_H_06_06_2007
#include <string>

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Camera.h"
#include "vision/include/Export.h"

// Forward Declartions
struct CvCapture; // For OpenCV

namespace ram {
namespace vision {

class RAM_EXPORT OpenCVCamera : public Camera
{
public:
    /** Opens Default Camera */
    OpenCVCamera();
    
    /** Open specific camera */
    OpenCVCamera(int camNum, bool forward);

    /** Open a specific movie */
    OpenCVCamera(std::string movieName);

    
    ~OpenCVCamera();

    /** This grabs the new image, and then stores it for Camera::getImage */
    virtual void update(double timestep);
	
    virtual size_t width();
    
    virtual size_t height();

    virtual double fps();

    virtual double duration();

    virtual void seekToTime(double seconds);

protected:
    /** Preforms a calbration during the copy if possible
     *  
     *  If m_calibration is non 0, it will perform the calibration and copy in
     *  one operation.  If not, it will use the standard camera copy.
     */
    virtual void copyToPublic(Image* newImage, Image* publicImage);
    
private:
    /** Retrieves the latest image from camera and undistorts
     *
     *	@param undistorted
     *      The image to place the undistorted results into
     */
    void getCalibratedImage(Image* undistorted);
    
    /** A Calibration with parameters that can be set for this camera */
    Calibration* m_calibration;
    
    /** OpenCV Capture handle */
    CvCapture* m_camCapture;
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_OPENCVCAMERA_H_06_06_2007

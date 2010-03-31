/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/Utility.cxx
 */

// Library Include
#include "cv.h"
#include "cxcore.h"
#include <boost/filesystem.hpp>

// Project Includes
#include "vision/test/include/Utility.h"
#include "vision/include/Image.h"
#include "vision/include/OpenCVImage.h"

#include "math/include/Matrix2.h"

// System Includes
#ifdef RAM_POSIX
#include <unistd.h>
#else
#include <windows.h>
#endif


static boost::filesystem::path getImagesDir()
{
    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
    return root / "packages" / "vision" / "test" / "data" / "references";
}

namespace ram {
namespace vision {

void makeColor(vision::Image* image, unsigned char R, unsigned char G,
               unsigned char B)
{
    int size = image->getWidth() * image->getHeight() * 3;
    unsigned char* data = image->getData();
    for (int i = 0; i < size; i += 3)
    {
        // BGR
        data[i] = B;
        data[i + 1]  = G;
        data[i + 2]  = R;
    }
}

void drawSquare(vision::Image* image, int x, int y, int width, int height,
                double angle, CvScalar color)
{
    // Four unrotated or translated cordinates
    math::Vector2 upperLeft(-width/2, height/2);
    math::Vector2 upperRight(width/2, height/2);
    math::Vector2 lowerLeft(-width/2, -height/2);
    math::Vector2 lowerRight(width/2, -height/2);

    // Rotate points
    math::Matrix2 rotationMatrix;
    rotationMatrix.fromAngle(math::Degree(angle));

    upperLeft = rotationMatrix * upperLeft;
    upperRight = rotationMatrix * upperRight;
    lowerLeft = rotationMatrix * lowerLeft;
    lowerRight = rotationMatrix * lowerRight;

    // Load into OpenCV format and translate
    CvPoint pts[4];
    pts[0].x = (int)upperLeft.x + x;
    pts[0].y = (int)upperLeft.y + y;
    pts[1].x = (int)upperRight.x + x;
    pts[1].y = (int)upperRight.y + y;
    pts[2].x = (int)lowerRight.x + x;
    pts[2].y = (int)lowerRight.y + y;
    pts[3].x = (int)lowerLeft.x + x;
    pts[3].y = (int)lowerLeft.y + y;

    // Finally lets draw the image
    cvFillConvexPoly(image->asIplImage(), pts, 4, color);
}
    
void drawBin(vision::Image* image, int x, int y, int width,
             double angle, TestSuitType suitType)
{
    static OpenCVImage heart((getImagesDir() / "heart.png").string());
    static OpenCVImage spade((getImagesDir() / "spade.png").string());
    static OpenCVImage club((getImagesDir() / "club.png").string());
    static OpenCVImage diamond((getImagesDir() / "diamond.png").string());
    static OpenCVImage tank((getImagesDir() / "tank_bin.png").string());
    static OpenCVImage ship((getImagesDir() / "ship_bin.png").string());
    static OpenCVImage aircraft((getImagesDir() / "aircraft_bin.png").string());
    static OpenCVImage factory((getImagesDir() / "factory_bin.png").string());
    static Image* suit2Image[] = {&heart, &spade, &club, &diamond,
                                  &tank, &ship, &aircraft, &factory};

    
    assert((image->getWidth() == 640) && (image->getHeight() == 480) &&
           "Draw bin image must be 640x480");
    
    // Generate the height based on the width
    int height = (int)((double)width * 1.5);
    
    // Draw white first rectangle first
    drawSquare(image, x, y, width, height, angle, CV_RGB(255, 255, 255));

    // Draw black rectangle
    int innerWidth = width / 2;
    int innerHeight = width;

    drawSquare(image, x, y, innerWidth, innerHeight, angle, CV_RGB(0, 0, 0));

    // Draw suit if needed
    if ((suitType == Heart) || (suitType == Spade) || (suitType == Club) ||
        (suitType == Diamond))
    {
        assert((suitType >= Heart) && (suitType < Tank) && "Improper suit");
        static OpenCVImage scratchImage(640, 480);
        static OpenCVImage rotatedAndScaled(640, 480);
        
        // Get the proper image
        Image* desired = suit2Image[suitType];

        // Scale it down to proper image size 640x480
        cvResize(desired->asIplImage(), scratchImage.asIplImage());
        
        // Scale and rotate suit to match bin
        int desiredPixelWidth = (int)((double)innerWidth * 2.0/3.0);
        double scaleFactor = ((double)desiredPixelWidth) /
            ((double) desired->getWidth());

        int xTrans = x - 320;
        int yTrans = y - 240;
        vision::Image::transform(&scratchImage, &rotatedAndScaled,
                                 math::Degree(-angle), scaleFactor, xTrans,
                                 yTrans, 255, 255, 255);

        // Transform and blit image into the proper place
        vision::Image::blitImage(&rotatedAndScaled, image, image,
                                 255, 255, 255, 0, 0);
    }

    if ((suitType == Tank) || (suitType == Ship) || (suitType == Aircraft) ||
        (suitType == Factory))
    {
        assert((suitType >= Tank) && (suitType < None) && "Improper symbol");
        static OpenCVImage scratchImage(640, 480);
        static OpenCVImage rotatedAndScaled(640, 480);
        
        // Get the proper image
        Image* desired = suit2Image[suitType];

        // Scale it down to proper image size 640x320
        //cvResize(desired->asIplImage(), scratchImage.asIplImage());

        // Scale and rotate suit to match bin
        int desiredPixelWidth = (int)((double)width);
        double scaleFactor = ((double)desiredPixelWidth) /
            ((double) scratchImage.getWidth());

        int xTrans = x - 320;
        int yTrans = y - 240;
        vision::Image::transform(desired, &rotatedAndScaled,
                                 math::Degree(-angle), scaleFactor, xTrans,
                                 yTrans, 100, 100, 100);

        // Transform and blit image into the proper place
        vision::Image::blitImage(&rotatedAndScaled, image, image,
                                 100, 100, 100, 0, 0);
        
    }

}


void drawTarget(vision::Image* image, int x, int y, int height, int width)
{
    if (-1 == width)
        width = height;
    
    // Determine setup 
    double stripeWidth = ((double)width/10.0);
    double stripeXOffset = (double)width/2 - stripeWidth/2;
    double stripeYOffset = (double)height/2 - stripeWidth/2;


    // Left green section
    drawSquare(image, (int)(x - stripeXOffset), y, (int)stripeWidth, height,
	       0, CV_RGB(0, 255, 0));
        

    // Right green section
    drawSquare(image, (int)(x + stripeXOffset), y, (int)stripeWidth, height,
	       0, CV_RGB(0, 255, 0));


    // Top green section
    drawSquare(image, x, (int)(y - stripeYOffset), width, (int)stripeWidth,
	       0, CV_RGB(0, 255, 0));


    // Bottem green section
    drawSquare(image, x, (int)(y + stripeYOffset), width, (int)stripeWidth,
	       0, CV_RGB(0, 255, 0));
}

    
void drawCircle(vision::Image* image, int x, int y, int radius,
		CvScalar color)
{
    CvPoint center;
    center.x = x;
    center.y = y;
    cvCircle(image->asIplImage(), center, radius, color, -1);
}

void drawLine(vision::Image* image, int x1, int y1, int x2, int y2,
              int lineWidth, CvScalar color)
{
    CvPoint pt1;
    pt1.x = x1;
    pt1.y = y1;

    CvPoint pt2;
    pt2.x = x2;
    pt2.y = y2;

    cvLine(image->asIplImage(), pt1, pt2, color, lineWidth);
}

int getPid()
{
#ifdef RAM_POSIX
    return getpid();
#else
    return (int)GetCurrentThreadId();
#endif
}

    
} // namespace vision
} // namespace ram

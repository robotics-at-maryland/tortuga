/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/Utility.cxx
 */

// Library Include
#include "cxcore.h"

// Project Includes
#include "vision/test/include/Utility.h"
#include "vision/include/Image.h"

#include "math/include/Matrix2.h"

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
    
} // namespace vision
} // namespace ram

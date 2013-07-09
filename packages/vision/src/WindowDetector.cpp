/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/src/WindowDetector.cpp
 */

// STD Includes
#include <assert.h>
#include <math.h>
#include <set>
#include <iostream>

// Library Includes
#include <boost/foreach.hpp>
#include "cv.h"

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/EventHub.h"
#include "core/include/PropertySet.h"

#include "vision/include/Camera.h"
#include "vision/include/Color.h"
#include "vision/include/Events.h"
#include "vision/include/Image.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/ColorFilter.h"
#include "vision/include/WindowDetector.h"
#include "vision/include/WhiteBalance.h"
#include "vision/include/BuoyDetectorKate.h"

namespace ram {
namespace vision {

WindowDetector::WindowDetector(core::ConfigNode config,
                               core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0),
    m_redFilter(0),
    m_greenFilter(0),
    m_yellowFilter(0),
    m_blueFilter(0),
    m_blobDetector(config, eventHub)
{
    init(config);
}

WindowDetector::WindowDetector(Camera* camera) :
    cam(camera),
    m_redFilter(0),
    m_greenFilter(0),
    m_yellowFilter(0),
    m_blueFilter(0)
{
    init(core::ConfigNode::fromString("{}"));
}

WindowDetector::~WindowDetector()
{
    delete m_redFilter;
    delete m_greenFilter;
    delete m_yellowFilter;
    delete m_blueFilter;

    delete frame;
    delete tempFrame;
    delete redFrame;
    delete greenFrame;
    delete yellowFrame;
    delete blueFrame;
}

void WindowDetector::init(core::ConfigNode config)
{
    // Detection variables
    // NOTE: The property set automatically loads the value from the given
    //       config if its present, if not it uses the default value presented.
    core::PropertySetPtr propSet(getPropertySet());

    propSet->addProperty(config, false, "debug",
                         "Debug level", 2, &m_debug, 0, 2);

    propSet->addProperty(config, false, "maxAspectRatio",
                         "Maximum aspect ratio (width/height)",
                         1.1, &m_maxAspectRatio);

    propSet->addProperty(config, false, "minAspectRatio",
                         "Minimum aspect ratio (width/height)",
                         0.25, &m_minAspectRatio);

    propSet->addProperty(config, false, "minWidth",
                         "Minimum width for a blob",
                         50, &m_minWidth);

    propSet->addProperty(config, false, "minHeight",
                         "Minimum height for a blob",
                         50, &m_minHeight);

    propSet->addProperty(config, false, "minRelInnerWidth",
                        "Minimum width for the inside of the target",
                         0.1, &m_minRelInnerWidth, 0.0, 1.0);

    propSet->addProperty(config, false, "minRelInnerHeight",
                        "Minimum height for the inside of the target",
                         0.1, &m_minRelInnerHeight, 0.0, 1.0);

    propSet->addProperty(config, false, "maxCenterXDisagreement",
                        "Maximum distance between the window and background centers",
                        1000, &m_centerXDisagreement);

    propSet->addProperty(config, false, "maxCenterYDisagreement",
                        "Maximum distance between the window and background centers",
                        1000, &m_centerYDisagreement);

    propSet->addProperty(config, false, "minPixelPercentage",
                         "Minimum percentage of pixels / area",
                         0.1, &m_minPixelPercentage, 0.0, 1.0);

    propSet->addProperty(config, false, "maxPixelPercentage",
                         "Maximum percentage of pixels / area",
                         1.0, &m_maxPixelPercentage, 0.0, 1.0);

    propSet->addProperty(config, false, "innerMinPixelPercentage",
                         "Minimum percentage for inner background",
                         0.1, &m_innerMinPixelPercentage, 0.0, 1.0);

    propSet->addProperty(config, false, "erodeIterations",
                         "Number of times to erode the binary image",
                         0, &m_erodeIterations);

    propSet->addProperty(config, false, "dilateIterations",
                         "Number of times to dilate the binary image",
                         0, &m_dilateIterations);

    m_redFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_redFilter->addPropertiesToSet(propSet, &config,
                                    "RedL", "Red Luminance",
                                    "RedC", "Red Chrominance",
                                    "RedH", "Red Hue",
                                    0, 255, 0, 255, 0, 255);
    m_greenFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_greenFilter->addPropertiesToSet(propSet, &config,
                                      "GreenL", "Green Luminance",
                                      "GreenC", "Green Chrominance",
                                      "GreenH", "Green Hue",
                                      0, 255, 0, 255, 0, 255);
    m_yellowFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_yellowFilter->addPropertiesToSet(propSet, &config,
                                       "YellowL", "Yellow Luminance",
                                       "YellowC", "Yellow Chrominance",
                                       "YellowH", "Yellow Hue",
                                       0, 255, 0, 255, 0, 255);
    m_blueFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_blueFilter->addPropertiesToSet(propSet, &config,
                                     "BlueL", "Blue Luminance",
                                     "BlueC", "Blue Chrominance",
                                     "BlueH", "Blue Hue",
                                     0, 255, 0, 255, 0, 255);

    // Make sure the configuration is valid
    propSet->verifyConfig(config, true);
    
    // Working images
    frame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    tempFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    redFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    greenFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    yellowFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    blueFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);

}

void WindowDetector::update()
{
    cam->getImage(frame);
    processImage(frame);
}

bool WindowDetector::processColor(Image* input, Image* output,
                                  ColorFilter& filter,
                                  BlobDetector::Blob& outerBlob,
                                  BlobDetector::Blob& innerBlob)
{
    tempFrame->copyFrom(input);
    tempFrame->setPixelFormat(Image::PF_RGB_8);
    tempFrame->setPixelFormat(Image::PF_LCHUV_8);

    filter.filterImage(tempFrame, output);

    // Erode the image (only if necessary)
    IplImage* img = output->asIplImage();
    if (m_erodeIterations > 0) {
        cvErode(img, img, NULL, m_erodeIterations);
    }

    // Dilate the image (only if necessary)
    if (m_dilateIterations > 0) {
        cvDilate(img, img, NULL, m_dilateIterations);
    }

    m_blobDetector.processImage(output);
    BlobDetector::BlobList blobs = m_blobDetector.getBlobs();

    BOOST_FOREACH(BlobDetector::Blob blob, blobs)
    {
        // Sanity check blob
        double pixelPercentage = blob.getSize() /
            (double) (blob.getHeight() * blob.getWidth());


        double aspect = blob.getTrueAspectRatio();
        if (aspect <= m_maxAspectRatio &&
            aspect >= m_minAspectRatio &&
            m_minHeight <= blob.getHeight() &&
            m_minWidth <= blob.getWidth() &&
            m_minPixelPercentage <= pixelPercentage &&
            m_maxPixelPercentage >= pixelPercentage &&
            processBackground(tempFrame, filter, blob, innerBlob))
        {
            int outerCenterX = (blob.getMaxX() - blob.getMinX()) / 2 + blob.getMinX();
            int innerCenterX = (innerBlob.getMaxX() - innerBlob.getMinX()) /
                2 + innerBlob.getMinX();

            int outerCenterY = (blob.getMaxY() - blob.getMinY()) / 2 + blob.getMinY();
            int innerCenterY = (innerBlob.getMaxY() - innerBlob.getMinY()) /
                2 + innerBlob.getMinY();

            int centerXdiff = abs(outerCenterX - innerCenterX);
            int centerYdiff = abs(outerCenterY - innerCenterY);

            double relHeight = (double) innerBlob.getHeight() / blob.getHeight();
            double relWidth = (double) innerBlob.getWidth() / blob.getWidth();

            if(centerXdiff < m_centerXDisagreement &&
               centerYdiff < m_centerYDisagreement &&
               relHeight > m_minRelInnerHeight &&
               relWidth > m_minRelInnerWidth) {

                outerBlob = blob;
                return true;

            }
        }
    }

    return false;
}

bool WindowDetector::processBackground(Image *input, ColorFilter& filter, 
                                       BlobDetector::Blob& outerBlob,
                                       BlobDetector::Blob& innerBlob)
{
    unsigned char *buffer = new unsigned char[outerBlob.getWidth()*outerBlob.getHeight()*3];
    Image *innerFrame = Image::extractSubImage(
        input, buffer,
        outerBlob.getMinX(), outerBlob.getMinY(),
        outerBlob.getMaxX(), outerBlob.getMaxY());

    filter.inverseFilterImage(innerFrame);
    m_blobDetector.processImage(innerFrame);
    delete innerFrame;
    delete [] buffer;

    BlobDetector::BlobList bgBlobs = m_blobDetector.getBlobs();
    
    BOOST_FOREACH(BlobDetector::Blob blob, bgBlobs) {
        double pxPercentage = blob.getFillPercentage();
        if(pxPercentage >= m_innerMinPixelPercentage) {
            innerBlob = blob;
            return true;
        }
    }
    return false;
}


void WindowDetector::processImage(Image* input, Image* output)
{

//KATE
	Mat img = input->asIplImage();
	imshow("input image", img);

	blobfinder window;
        Mat output_blob= img;
	//IplImage* tempImage=0;
	window.FindWindow(input,output_blob, m_redFilter, m_greenFilter,m_yellowFilter);
       // output->setData(output_blob2.data,false);
        //tempImage->imageData = (char *)output_blob2.data;


    frame->copyFrom(input);

    BlobDetector::Blob redBlob, greenBlob, yellowBlob, blueBlob;
    BlobDetector::Blob innerRedBlob, innerGreenBlob, innerYellowBlob, innerBlueBlob;
    bool redFound = false, greenFound = false,
        yellowFound = false, blueFound = false;

    if ((redFound = processColor(frame, redFrame, *m_redFilter,
                                 redBlob, innerRedBlob))) {
        publishFoundEvent(redBlob, Color::RED);
    } else {
        // Publish lost event if this was found previously
        if (m_redFound) {
            publishLostEvent(Color::RED);
        }
    }
    m_redFound = redFound;

    if ((greenFound = processColor(frame, greenFrame, *m_greenFilter,
                                  greenBlob, innerGreenBlob))) {
        publishFoundEvent(greenBlob, Color::GREEN);
    } else {
        // Publish lost event if this was found previously
        if (m_greenFound) {
            publishLostEvent(Color::GREEN);
        }
    }
    m_greenFound = greenFound;

    if ((yellowFound = processColor(frame, yellowFrame, *m_yellowFilter,
                                    yellowBlob, innerYellowBlob))) {
        publishFoundEvent(yellowBlob, Color::YELLOW);
    } else {
        // Publish lost event if this was found previously
        if (m_yellowFound) {
            publishLostEvent(Color::YELLOW);
        }
    }
    m_yellowFound = yellowFound;

    if ((blueFound = processColor(frame, blueFrame, *m_blueFilter,
                                  blueBlob, innerBlueBlob))) {
        publishFoundEvent(blueBlob, Color::BLUE);
    } else {
        // Publish lost event if this was found previously
        if (m_blueFound) {
            publishLostEvent(Color::BLUE);
        }
    }
    m_blueFound = blueFound;

    if (output)
    {
        if (m_debug == 0) {
            output->copyFrom(frame);
        } else {
            output->copyFrom(frame);
            
            Image::fillMask(output, redFrame, 255, 0, 0);
            Image::fillMask(output, greenFrame, 0, 255, 0);
            Image::fillMask(output, yellowFrame, 255, 255, 0);
            Image::fillMask(output, blueFrame, 0, 0, 255);
            // // Color in the targets (as we find them)
            // unsigned char* data = output->getData();
            // unsigned char* redPtr = redFrame->getData();
            // unsigned char* greenPtr = greenFrame->getData();
            // unsigned char* yellowPtr = yellowFrame->getData();
            // unsigned char* bluePtr = blueFrame->getData();
            // unsigned char* end = output->getData() +
            //     (output->getWidth() * output->getHeight() * 3);
            
            // for (; data != end; data += 3) {
            //     if (*redPtr) {
            //         data[0] = 0;
            //         data[1] = 0;
            //         data[2] = 255;
            //     } else if (*greenPtr) {
            //         data[0] = 0;
            //         data[1] = 255;
            //         data[2] = 0;
            //     } else if (*yellowPtr) {
            //         data[0] = 0;
            //         data[1] = 255;
            //         data[2] = 255;
            //     } else if (*bluePtr) {
            //         data[0] = 255;
            //         data[1] = 0;
            //         data[2] = 0;
            //     }

            //     // Advance all of the pointers
            //     redPtr += 3;
            //     greenPtr += 3;
            //     yellowPtr += 3;
            //     bluePtr += 3;
            // }
            
            if (m_debug == 2) {
                if (redFound) {
                    CvPoint center;
                    center.x = redBlob.getCenterX();
                    center.y = redBlob.getCenterY();

                    redBlob.drawStats(output);

                    CvPoint p1;
                    p1.x = redBlob.getMinX() + innerRedBlob.getMinX();
                    p1.y = redBlob.getMinY() + innerRedBlob.getMinY();

                    CvPoint p2;
                    p2.x = redBlob.getMinX() + innerRedBlob.getMaxX();
                    p2.y = redBlob.getMinY() + innerRedBlob.getMaxY();

                    cvRectangle(output->asIplImage(),p1,p2,cvScalar(100,100,100),3);
                    cvRectangle(output->asIplImage(),
                                cvPoint(redBlob.getMinX(),redBlob.getMinY()),
                                cvPoint(redBlob.getMaxX(),redBlob.getMaxY()),
                                cvScalar(255,255,255),2);
                    cvCircle(output->asIplImage(), center, 3,
                             cvScalar(0, 0, 150), -1);

                }

                if (greenFound) {
                    CvPoint center;
                    center.x = greenBlob.getCenterX();
                    center.y = greenBlob.getCenterY();

                    greenBlob.drawStats(output);

                    CvPoint p1;
                    p1.x = greenBlob.getMinX() + innerGreenBlob.getMinX();
                    p1.y = greenBlob.getMinY() + innerGreenBlob.getMinY();

                    CvPoint p2;
                    p2.x = greenBlob.getMinX() + innerGreenBlob.getMaxX();
                    p2.y = greenBlob.getMinY() + innerGreenBlob.getMaxY();

                    cvRectangle(output->asIplImage(),p1,p2,cvScalar(100,100,100),3);
                    cvRectangle(output->asIplImage(),
                                cvPoint(greenBlob.getMinX(),greenBlob.getMinY()),
                                cvPoint(greenBlob.getMaxX(),greenBlob.getMaxY()),
                                cvScalar(255,255,255),2);
                    cvCircle(output->asIplImage(), center, 3,
                             cvScalar(0, 150, 0), -1);
                }

                if (yellowFound) {
                    CvPoint center;
                    center.x = yellowBlob.getCenterX();
                    center.y = yellowBlob.getCenterY();

                    yellowBlob.drawStats(output);

                    CvPoint p1;
                    p1.x = yellowBlob.getMinX() + innerYellowBlob.getMinX();
                    p1.y = yellowBlob.getMinY() + innerYellowBlob.getMinY();

                    CvPoint p2;
                    p2.x = yellowBlob.getMinX() + innerYellowBlob.getMaxX();
                    p2.y = yellowBlob.getMinY() + innerYellowBlob.getMaxY();

                    cvRectangle(output->asIplImage(),p1,p2,cvScalar(100,100,100),3);
                    cvRectangle(output->asIplImage(),
                                cvPoint(yellowBlob.getMinX(),yellowBlob.getMinY()),
                                cvPoint(yellowBlob.getMaxX(),yellowBlob.getMaxY()),
                                cvScalar(255,255,255),2);
                    // Yellow = Red + Green
                    cvCircle(output->asIplImage(), center, 3,
                             cvScalar(0, 150, 150), -1);
                }

                if (blueFound) {
                    CvPoint center;
                    center.x = blueBlob.getCenterX();
                    center.y = blueBlob.getCenterY();

                    blueBlob.drawStats(output);

                    CvPoint p1;
                    p1.x = blueBlob.getMinX() + innerBlueBlob.getMinX();
                    p1.y = blueBlob.getMinY() + innerBlueBlob.getMinY();

                    CvPoint p2;
                    p2.x = blueBlob.getMinX() + innerBlueBlob.getMaxX();
                    p2.y = blueBlob.getMinY() + innerBlueBlob.getMaxY();

                    cvRectangle(output->asIplImage(),p1,p2,cvScalar(100,100,100),3);
                    cvRectangle(output->asIplImage(),
                                cvPoint(blueBlob.getMinX(),blueBlob.getMinY()),
                                cvPoint(blueBlob.getMaxX(),blueBlob.getMaxY()),
                                cvScalar(255,255,255),2);
                    cvCircle(output->asIplImage(), center, 3,
                             cvScalar(150, 0, 0), -1);
                }
            }
        }
    }
}

void WindowDetector::show(char* window)
{
    vision::Image::showImage(frame, "WindowDetector");
}

IplImage* WindowDetector::getAnalyzedImage()
{
    return frame->asIplImage();
}

void WindowDetector::publishFoundEvent(const BlobDetector::Blob& blob,
                                       Color::ColorType color)
{
    WindowEventPtr event(new WindowEvent());
    event->color = color;

    Detector::imageToAICoordinates(frame,
                                   blob.getCenterX(),
                                   blob.getCenterY(),
                                   event->x,
                                   event->y);

    // Determine range
    event->range = 1.0 - (((double)blob.getHeight()) /
                          ((double)frame->getHeight()));

    // Determine the squareness
    double aspectRatio = blob.getTrueAspectRatio();
    if (aspectRatio < 1)
        event->squareNess = 1.0;
    else
        event->squareNess = 1.0/aspectRatio;

    publish(EventType::WINDOW_FOUND, event);
}

void WindowDetector::publishLostEvent(Color::ColorType color)
{
    WindowEventPtr event(new WindowEvent());
    event->color = color;
    
    publish(EventType::WINDOW_LOST, event);
}

} // namespace vision
} // namespace ram

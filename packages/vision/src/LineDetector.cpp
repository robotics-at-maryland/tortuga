/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/src/LineDetector.cpp
 */

// STD Includes
#include <algorithm>

// Project Includes
#include "core/include/PropertySet.h"
#include "vision/include/Image.h"
#include "vision/include/LineDetector.h"
#include "vision/include/OpenCVImage.h"

namespace ram {
namespace vision {

LineDetector::LineDetector(core::ConfigNode config,
                           core::EventHubPtr eventHub,
                           HoughType type) :
    Detector(eventHub),
    m_minLineSize(0),
    m_houghType(type),
    data(0),
    m_dataSize(0)
{
    init(config);
}

LineDetector::LineDetector(int minimumLineSize, HoughType type) :
    Detector(core::EventHubPtr()),
    m_minLineSize(minimumLineSize),
    m_houghType(type),
    data(0),
    m_dataSize(0)
{
}

LineDetector::~LineDetector()
{
    delete data;
}

void LineDetector::init(core::ConfigNode config)
{
    core::PropertySetPtr propSet(getPropertySet());
    propSet->addProperty(config, false, "highThreshold",
                         "High threshold for canny edge detector",
                         200.0, &m_highThreshold, 0.0, 255.0);

    propSet->addProperty(config, false, "lowThreshold",
                         "Low threshold for canny edge detector",
                         .4*200, &m_lowThreshold, 0.0, 255.0);

    propSet->addProperty(config, false, "houghThreshold",
                         "Threshold for the hough transform",
                         100, &m_houghThreshold, 0, 255);

    propSet->addProperty(config, false, "maxLines",
                         "Maximum number of lines to find",
                         10, &m_maxLines);

    propSet->addProperty(config, false, "minLineSize",
                         "Minimum size of a line",
                         0.0, &m_minLineSize);

    propSet->addProperty(config, false, "minGapLength",
                         "Maximum gap between two lines to be "
                         "considered the same", 0.0, &m_maxGapLength);

    propSet->verifyConfig(config, true);
}

void LineDetector::processImage(Image* input, Image* output)
{
    // Grayscale images only
    assert(input->getPixelFormat() == Image::PF_GRAY_8 &&
           "LineDetector: processImage(): input must be a grayscale image");
    ensureDataSize(input->getWidth(), input->getHeight());
   
    cvCanny(input->asIplImage(), data->asIplImage(),
            m_lowThreshold, m_highThreshold);
    houghTransform();

    // Tag lines that are too short
    double minLength = m_minLineSize*m_minLineSize;
    std::vector<int> taggedLines;
    int i = 0;
    BOOST_FOREACH(LineDetector::Line line, m_lines)
    {
        if (line.squaredLength() < minLength) {
            taggedLines.push_back(i);
        }
        i += 1;
    }

    // Now remove them
    int offset = 0;
    BOOST_FOREACH(int iter, taggedLines)
    {
        m_lines.erase(m_lines.begin()+(iter - offset));
        offset += 1;
    }

    // Debug stuff
    if (0 != output)
    {
        //output->copyFrom(input);
        output->copyFrom(data);

        BOOST_FOREACH(LineDetector::Line line, m_lines)
        {
            if (m_houghType != STANDARD) {
                CvPoint pt1, pt1BoundUR, pt1BoundLL;
                CvPoint pt2, pt2BoundUR, pt2BoundLL;
                int boxsize = 5;
                int linesize = 3;

                pt1 = line.point1();
                pt1BoundUR.x = pt1.x + boxsize;
                pt1BoundUR.y = pt1.y - boxsize;
                pt1BoundLL.x = pt1.x - boxsize;
                pt1BoundLL.y = pt1.y + boxsize;
                pt2 = line.point2();
                pt2BoundUR.x = pt2.x + boxsize;
                pt2BoundUR.y = pt2.y - boxsize;
                pt2BoundLL.x = pt2.x - boxsize;
                pt2BoundLL.y = pt2.y + boxsize;

                // Draw a small box around the points
                cvRectangle(output->asIplImage(), pt1BoundUR, pt1BoundLL,
                            CV_RGB(0,255,0), 2, CV_AA, 0);
                cvRectangle(output->asIplImage(), pt2BoundUR, pt2BoundLL,
                            CV_RGB(0,255,0), 2, CV_AA, 0);

                // Draw the line
                cvLine(output->asIplImage(), pt1, pt2,
                       CV_RGB(0,0,255), linesize);
            }
        }
    }
}

int LineDetector::houghTransform()
{
    // Create memory storage for cvHoughLines2
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* lines = 0;

    int method;
    if (m_houghType == STANDARD) {
        method = CV_HOUGH_STANDARD;
    } else {
        method = CV_HOUGH_PROBABILISTIC;
    }

    // ThetaThreshold = 0.5, RhoThreshold = 0.5
    lines = cvHoughLines2(data->asIplImage(),
                          storage,
                          method,
                          0.5, CV_PI/360, m_houghThreshold,
                          m_minLineSize, m_maxGapLength); // Ignored if STANDARD

    // Convert these into our format
    for (int i = 0; i < std::min(lines->total, m_maxLines); i++ )
    {
        float rho; // Replace with NaN
        float theta; // Replace with NaN
        CvPoint pt1; // Replace with ?
        CvPoint pt2; // Replace with ?
        
        // Copied from OpenCV documentation

        if (m_houghType == STANDARD) {
            float* line = (float *) cvGetSeqElem(lines, i);
            rho = line[0];
            theta = line[1];
        } else {
            CvPoint* line = (CvPoint *) cvGetSeqElem(lines, i);
            pt1 = line[0];
            pt2 = line[1];
        
            if (m_houghType == BOTH) {
                // TODO: Figure out how to do this
                /**
                   y = mx + b
                   m = rise/run
                   m = (y2 - y1) / (x2 - x1)
                   m = -(cos(theta)/sin(theta))
                   b = y - mx
                   
                   b = rho/sin(theta)
                   m = -(cos(theta)/sin(theta))
                   // y = -(cos(theta)/sin(theta))*x + rho/sin(theta)
                   // rho = (y + (cos(theta)/sin(theta)))*sin(theta)
                   float rho = (y + (cos(
                */
            }
        }

        m_lines.push_back(
            LineDetector::Line(pt1, pt2, math::Radian(theta), rho));
    }

    return m_lines.size();
}

void LineDetector::ensureDataSize(size_t width, size_t height)
{
    // Don't resize if the image is the correct size
    if (data && data->getWidth() == width && data->getHeight() == height) {
        return;
    }

    // Delete old image and create new one
    delete data;
    data = new OpenCVImage(width, height, Image::PF_GRAY_8);
}

void LineDetector::setMinimumLineSize(int pixels)
{
    m_minLineSize = pixels;
}

int LineDetector::getMinimumLineSize()
{
    return m_minLineSize;
}

std::vector<LineDetector::Line> LineDetector::getLines()
{
    return m_lines;
}

} // namespace vision
} // namespace ram

/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Chris Giles
 * All rights reserved.
 *
 * Author: Chris Giles <cgiles17@umd.edu>
 * File:  packages/vision/include/DuctDetector.cpp
 */

// STD Includes
#include <algorithm>
#include <cmath>
#include <iostream>
#include <math.h>

// Library Includes
//#include "highgui.h"

// Project Includes
#include "vision/include/Camera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/DuctDetector.h"
#include "vision/include/Events.h"
#include "vision/include/main.h"

#ifndef M_PI
#define M_PI 3.14159
#endif

//#include "vision/include/DuctDetector.h"

namespace ram {
namespace vision {

int DuctDetector::yellow(unsigned char r, unsigned char g, unsigned char b)
{
if (r > m_redThreshold && g > m_greenThreshold && b < m_blueThreshold)
    return 1;
return 0;

}

double min(double a, double b)
{
    if (a < b)
        return a;
    return b;
}

double max(double a, double b)
{
    if (a > b)
        return a;
    return b;
}

DuctDetector::DuctDetector(core::ConfigNode config,
                           core::EventHubPtr eventHub) :
    m_working(new OpenCVImage(480, 640)),
    m_x(0.0),
    m_y(0.0),
    m_rotation(M_PI/2.0),
    n_x(0.0),
    n_y(0.0)
{
    init(config);
}
    
DuctDetector::DuctDetector(core::EventHubPtr eventHub) :
    Detector(eventHub),
    m_working(new OpenCVImage(480, 640)),
    m_x(0.0),
    m_y(0.0),
    m_rotation(M_PI/2.0),
    n_x(0.0),
    n_y(0.0)
{
    init(core::ConfigNode::fromString("{}"));
}

DuctDetector::~DuctDetector()
{
    delete m_working;
}

void DuctDetector::init(core::ConfigNode config)
{
    m_redThreshold = config["redThreshold"].asInt(100);
    m_greenThreshold = config["greenThreshold"].asInt(100);
    m_blueThreshold = config["blueThreshold"].asInt(90);
    m_erodeIterations = config["erodeIterations"].asInt(1);
}
    
    
void DuctDetector::processImage(Image* input, Image* output)
{   
    // Rotate image 90 degrees counter clockwise to account for camera crap
    if ((m_working->getWidth() != input->getHeight()) ||
        (m_working->getHeight() != input->getWidth()))
    {
        // Resize if needed
        delete m_working;
        m_working = new OpenCVImage(input->getHeight(), input->getWidth());
    }
    m_working->copyFrom(input);

    // Grab data pointers
    unsigned char* data = m_working->getData();
    unsigned char* outputData = 0;

    if (output)
    {
        output->copyFrom(m_working);
        outputData = output->getData();
    }


    int width = m_working->getWidth();
    int height = m_working->getHeight();
        
    int minX = 1000, minY = 1000, maxX = -10000, maxY = -10000;

    // Make all yellow white, everything else
    for (int j=0;j<height;j++)
    {
        int offset = j*width*3;
        for (int i=0;i<width*3;i+=3)
        {
            data[offset+i+2] = data[offset+i+1] = data[offset+i] =
                (yellow(data[offset+i+2], // R
                        data[offset+i+1], // G
                        data[offset+i])
                 * 255);  // B
        }
    }

//    for (int i=0;i<2;i++)
    m_erodeIterations = 2;
    if (m_erodeIterations != 0)
    {
        cvErode(m_working->asIplImage(), m_working->asIplImage(), 0,
                m_erodeIterations);
    }

    if (output)
    {
        output->copyFrom(m_working);
        outputData = output->getData();
    }
    
    for (int j=0;j<height;j++)
    {
        int offset = j*width*3;
        for (int i=0;i<width*3;i+=3)
        {
            if (data[offset+i] != 0)
            {
                if (i/3 < minX) minX = i/3;
                if (i/3 > maxX) maxX = i/3;
                if (j < minY) minY = j;
                if (j > maxY) maxY = j;
            }
        }
    } 
    
    double oldMx = m_x;
    m_range = 1 - (((double)(maxX - minX)) / width);
    
    m_x = (minX + maxX) * 0.5;
    m_y = (minY + maxY) * 0.5;
    
    if (m_x < 0)
    {
        if (oldMx > 0)
        {
            DuctEventPtr event(new DuctEvent(0, 0, 0, 0, false, false));
            publish(EventType::DUCT_LOST, event);
        }
        oldMx = -1000;
        m_rotation = 10;
        return;
    }
    
    if (m_working == 0)
        return;
    
    
    IplImage* redSuitGrayScale = cvCreateImage(cvGetSize(m_working->asIplImage()),
                                               IPL_DEPTH_8U,1);

    cvCvtColor(m_working->asIplImage(),redSuitGrayScale,CV_BGR2GRAY);
    IplImage* cannied = cvCreateImage(cvGetSize(redSuitGrayScale), 8, 1 );

    cvCanny( redSuitGrayScale, cannied, 50, 200, 3 );
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* lines = 0;
        
    lines = cvHoughLines2( cannied, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 10, 70, 30 );
    
    CvPoint* maxLine = 0;
    int mY = 0;
    
    for(int i = 0; i < lines->total; i++ )
    {
        CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
        if (max(line[0].y, line[1].y) > mY &&
            sqrt(pow(line[0].x - line[1].x, 2) +
                 pow(line[0].y - line[1].y, 2)) > 100)
        {
            if (fabs((double)(line[1].y - line[0].y) /
                 (double)(line[1].x - line[0].x)) < 0.5)
                maxLine = line;
            mY = (int)max(line[0].y, line[1].y);
        }
        
        if (output)
        {
            IplImage* raw = output->asIplImage();
            cvLine(raw, line[0], line[1], CV_RGB(0,255,0), 3, CV_AA, 0 );
        }
    }
    
    if (maxLine != 0 && maxLine[1].x != maxLine[0].x)
    {
        if (output)
        {
            IplImage* raw = output->asIplImage();
            cvLine(raw, maxLine[0], maxLine[1], CV_RGB(0,0,255), 3, CV_AA, 0 );
        }
        m_rotation = (double)(maxLine[1].y - maxLine[0].y) / (double)(maxLine[1].x - maxLine[0].x);
        std::cout << m_rotation << "\n";
    }
    
    cvReleaseImage(&redSuitGrayScale);
    cvReleaseImage(&cannied);
    cvReleaseMemStorage(&storage);
    
        
    if (output && m_x > 0)
    {
       // Color all yellow pixels white
/*        unsigned char* odata = output->getData();
        unsigned char* idata = m_working->getData();
        size_t pixels = output->getWidth() * output->getHeight();
        for (size_t i = 0; i < pixels; ++i)
        {
            if (*idata > 0) //yellow(*(idata+2), *(idata+1), *idata))
            {
                *(odata++) = 0;
                *(odata++) = 255;
                *(odata++) = 0;
            }
            else
            {
                odata += 3;
            }
            idata += 3;
            }*/
        
        // Draw bounding box
        CvPoint tl,tr,bl,br;
        tl.x = bl.x = std::max(minX,0) - 1;
        tr.x = br.x = std::min(maxX,width-1) + 1;
        tl.y = tr.y = std::min(minY,height-1) + 1;
        br.y = bl.y = std::max(maxY,0) - 1;
            
        IplImage* raw = output->asIplImage();
        cvLine(raw, tl, tr, CV_RGB(0,0,255), 3, CV_AA, 0 );
        cvLine(raw, tl, bl, CV_RGB(0,0,255), 3, CV_AA, 0 );
        cvLine(raw, tr, br, CV_RGB(0,0,255), 3, CV_AA, 0 );
        cvLine(raw, bl, br, CV_RGB(0,0,255), 3, CV_AA, 0 );
        
        // Draw center of bin
        CvPoint binCenter;
        binCenter.x = (int)m_x;
        binCenter.y = (int)m_y;
        if (getAligned() && getVisible())
            cvCircle(raw, binCenter, 10, CV_RGB(0,255,0), 2, CV_AA, 0);
        else
            cvCircle(raw, binCenter, 10, CV_RGB(255,0,0), 2, CV_AA, 0);

        // Draw rotation indicator
        CvPoint rotationEnd;
        rotationEnd.y = binCenter.y;
        rotationEnd.x = binCenter.x +
            (int)((m_rotation/-90) * (double)width/2.0) * 10;
        if (getAligned() && getVisible())
            cvLine(raw, binCenter, rotationEnd, CV_RGB(0,255,0), 3, CV_AA, 0 );
        else
            cvLine(raw, binCenter, rotationEnd, CV_RGB(255,0,0), 3, CV_AA, 0 );
    }
    
    n_x = -1 * ((width / 2) - m_x);
    n_y = (height / 2) - m_y;
    n_x = n_x / ((double)width) * 2.0;
    n_y = n_y / ((double)height) * 2.0;
    n_x *= (double)width/height;
        
        
    DuctEventPtr event(new DuctEvent(n_x, n_y, m_range, m_rotation, 
        getAligned(), getVisible()));
    publish(EventType::DUCT_FOUND, event);
}
    
double DuctDetector::getX()
{
    return n_x;
}

double DuctDetector::getY()
{
    return n_y;
}

double DuctDetector::getRange()
{
    return m_range;
}
    
double DuctDetector::getRotation()
{
    return m_rotation;
}

bool DuctDetector::getVisible()
{
    return m_x >= 0;
}
    
bool DuctDetector::getAligned()
{
    return fabsf(m_rotation) < 0.02;
}

} // namespace vision
} // namespace ram


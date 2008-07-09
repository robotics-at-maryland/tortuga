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

int yellow(unsigned char r, unsigned char g, unsigned char b)
{
if (r > 120 && g > 120 && b < 30)
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

DuctDetector::DuctDetector(core::EventHubPtr eventHub) :
    Detector(eventHub)
{
    m_x = m_y = 0.0;
    m_rotation = M_PI/2.0;
}
    
void DuctDetector::processImage(Image* input, Image* output)
{
    unsigned char* data = input->getData();
    //unsigned char* outputData = output->getData();
    
    if (output)
    {
        output->copyFrom(input);
    }
    
    int *hist = new int[(int)input->getWidth()];
    
    for (int i=0;i<(int)input->getWidth();i++)
        hist[i] = 0;
        
    int minX = 1000, minY = 1000, maxX = -10000, maxY = -10000;
    
    for (int j=0;j<(int)input->getHeight();j++)
    {
        int offset = j*input->getWidth()*3;
        for (int i=0;i<(int)input->getWidth()*3;i+=3)
        {
            int y = yellow(data[offset+i+2], // R
                           data[offset+i+1], // G
                           data[offset+i]);  // B
            if (y > 0)
            {
                if (i/3 < minX) minX = i/3;
                if (i/3 > maxX) maxX = i/3;
                if (j < minY) minY = j;
                if (j > maxY) maxY = j;
            }
            hist[i/3] += y;
        }
    } 
    
    double oldMx = m_x;
    m_size = maxX - minX;
    m_x = (minX + maxX) * 0.5;
    m_y = (minY + maxY) * 0.5;
    
    if (m_x < 0)
    {
        if (oldMx > 0)
        {
            DuctEventPtr event(new DuctEvent(0, 0, 0, 0, false, false));
            publish(EventType::DUCT_LOST, event);
        }
        delete[] hist;
        return;
    }
    
    
    double ps[4];
    ps[0] = ps[1] = -1000;
    ps[2] = ps[3] = 1000;
    int index = 0;
    bool found = false;
    
    for (int i=0;i<(int)input->getWidth();i++)
    {
        if (hist[i] > 1 && !found)
        {
            ps[index] = i / (double)input->getWidth();
            index ++;
            found = true;
        }
        else if (hist[i] <= 1 && found)
        {
            ps[index] = i / (double)input->getWidth();
            index ++;
            found = false;
        }
        if (index >= 4)
            break;
    }
    
    
    
    if (ps[2] > 1 && ps[3] > 1)
    {
        int offset = static_cast<int>(m_y*input->getWidth()*3);
        ps[0] = ps[1] = ps[2] = ps[3] = -10000;
        int ind = 0;
        bool found = false;
        
        for (int i=0;i<(int)input->getWidth()*3;)
        {
            if (!found && yellow(data[offset+i+2], data[offset+i+1], 
                                 data[offset+i]))
            {
                ps[ind] = i/3;
                //outputData[offset+i+2] = 255;
                //outputData[offset+i+1] = 0;
                //outputData[offset+i+0] = 0;
                ind++;
                found = true;
            }
            else if (found && !yellow(data[offset+i+2], data[offset+i+1], 
                                      data[offset+i]))
            {
                found = false;
            }
            
            if (ind == 4) break;
            
            if (found)
                i += 3*4;
            else
                i += 3;
        }
        
        
        if (ps[0] < 0 || ps[1] < 0 || ps[2] < 0 || ps[3] < 0)
        {
            m_rotation = 0;
        }
        else
        {
            if (ps[3] - ps[2] < ps[1] - ps[0])
            {
                m_rotation = ( 1.0 - (double)(ps[3] - ps[2]) / 
                    (double)(ps[1] - ps[0]) ) * 10;
            }
            else
            {
                m_rotation = ( 1.0 - (double)(ps[1] - ps[0]) / 
                    (double)(ps[3] - ps[2]) ) * 10;
            }
        }
    }
    else
    {
        m_rotation = 2000 * (ps[3] - ps[2]);
    }
        
    double ny = 2.0 * (m_x / 640 - 0.5) * 640.0/480.0;
    double nx = 2.0 * (m_y / 480 - 0.5);
    m_size = m_size / (double)input->getWidth();
        
    DuctEventPtr event(new DuctEvent(nx, ny, m_size, m_rotation, 
        getAligned(), getVisible()));
    publish(EventType::DUCT_FOUND, event);

    delete[] hist;
}
    
double DuctDetector::getX()
{
    return 2.0 * (m_y / 480 - 0.5);
}

double DuctDetector::getY()
{
    return 2.0 * (m_x / 640 - 0.5) * 640.0/480.0;
}

double DuctDetector::getSize()
{
    return m_size;
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
    return fabs(m_rotation) <= 2;
}

} // namespace vision
} // namespace ram

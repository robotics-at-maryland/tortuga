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
	
	int *hist = new int[(int)input->getWidth()];
	
	for (int i=0;i<(int)input->getWidth();i++)
		hist[i] = 0;
	
	for (int j=0;j<(int)input->getHeight();j++)
	{
		int offset = j*input->getWidth()*3;
		for (int i=0;i<(int)input->getWidth()*3;i+=3)
                {
                    hist[i/3] += yellow(data[offset+i+2], // R
                                        data[offset+i+1], // G
                                        data[offset+i]);  // B
                }
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
		m_rotation = 0;
        }
	else
        {
            m_rotation = (1.0 - min(ps[3] - ps[1], ps[1] - ps[0]) /
                          max(ps[3] - ps[1], ps[1] - ps[0])) * 90;

        }
	delete[] hist;
}
	
double DuctDetector::getX()
{
	return m_x;
}

double DuctDetector::getY()
{
	return m_y;
}
	
double DuctDetector::getRotation()
{
	return m_rotation;
}
	
bool DuctDetector::getAligned()
{
	return fabs(m_rotation) < 0.01;
}

} // namespace vision
} // namespace ram

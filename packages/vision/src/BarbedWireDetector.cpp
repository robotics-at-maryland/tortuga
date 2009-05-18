/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/BarbedWireDetector.cpp
 */

// STD Includes
#include <algorithm>
#include <cmath>
#include <sstream>

// Library Includes
#include "cv.h"
#include "highgui.h"
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

// Project Includes
#include "vision/include/BarbedWireDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"
#include "vision/include/ColorFilter.h"

#include "core/include/PropertySet.h"


namespace ram {
namespace vision {

BarbedWireDetector::BarbedWireDetector(core::ConfigNode config,
                                       core::EventHubPtr eventHub) :
    PipeDetector(config, eventHub, 500, 400),
    m_filter(new ColorFilter(0, 255, 0, 255, 0, 255)),
    m_image(new OpenCVImage(640, 480)),
    m_found(false),
    m_minAngle(0),
    m_erodeIterations(0),
    m_dilateIterations(0),
    m_topRemovePercentage(0),
    m_bottomRemovePercentage(0)
{
    init(config);
}
    
void BarbedWireDetector::init(core::ConfigNode config)
{    
    // Detection variables
    // NOTE: The property set automatically loads the value from the given
    //       config if its present, if not it uses the default value presented.
    core::PropertySetPtr propSet(getPropertySet());

    // Hack properties to deal noise from the surface of the water
    propSet->addProperty(config, false, "topRemovePercentage",
        "% of the screen from the top to be blacked out",
        0.0, &m_topRemovePercentage, 0.0, 1.0);

    propSet->addProperty(config, false, "bottomRemovePercentage",
        "% of the screen from the bottom to be blacked out",
        0.0, &m_bottomRemovePercentage, 0.0, 1.0);

    // Standard tuning properties
    propSet->addProperty(config, false, "minAngle",
        "The minimum angle a barbed wire pipe can have (Degrees)",
        40.0, &m_minAngle, 0.0, 90.0);

    propSet->addProperty(config, false, "erodeIterations",
        "How many times to erode the filtered image",
	0, &m_erodeIterations, 0, 10);

    propSet->addProperty(config, false, "dilateIterations",
        "How many times to dilate the filtered image",
         0, &m_dilateIterations, 0, 10);

    // Color filter properties
    m_filter->addPropertiesToSet(propSet, &config,
                                 "Y", "Luma",
                                 "U", "Blue Chrominance",
                                 "V", "Red Chrominance",
                                 1, 255,  // Y defaults
                                 0, 132,  // U defaults (80,141)
                                 0, 119); // V defaults (98, 119)

    /// TODO: add a found pixel drop off
}
    
BarbedWireDetector::~BarbedWireDetector()
{
    delete m_image;
}

bool BarbedWireDetector::found()
{
    return m_found;
}

void BarbedWireDetector::setTopRemovePercentage(double percent)
{
    m_topRemovePercentage = percent;
}

void BarbedWireDetector::setBottomRemovePercentage(double percent)
{
    m_bottomRemovePercentage = percent;
}
    
void BarbedWireDetector::processImage(Image* input, Image* output)
{
    // Ensure our working image is the same size
    if ((m_image->getWidth() != input->getWidth()) || 
        (m_image->getHeight() != input->getHeight()))
    {
        m_image->copyFrom(input);
    }

    // Copy to our working image and convert to YUV
    cvCvtColor(input->asIplImage(), m_image->asIplImage(), CV_BGR2YCrCb);

    // Filter for green
    filterForGreen(m_image);

    // Do the debug display of the filtering
    if (output)
    {
        // Make the output exactly match the input
        output->copyFrom(input);

	// Color all found pixels pink
	unsigned char* inData = m_image->getData();
	unsigned char* outData = output->getData();
	size_t numPixels = input->getHeight() * input->getWidth();
	
	for (size_t i = 0; i < numPixels; ++i)
        {
            if ((*inData))
	    {
	        *outData = 147; // B
		*(outData + 1) = 20; // G
		*(outData + 2) = 255; // R
	    }
	    
	    inData += 3;
	    outData += 3;
	}
    }

    
    // Find all of our pipes
    PipeDetector::processImage(m_image, output);

    // Filter for pipes of the right angle
    PipeDetector::PipeList candidatePipes = getPipes();
    PipeDetector::PipeList pipes;
    processPipes(candidatePipes, pipes);

    // Determine if we have found anything
    if (pipes.size() > 0)
    {
        m_found = true;
    }
    else
    {
        // Just lost the light so issue a lost event
        if (m_found)
            publish(EventType::BARBED_WIRE_LOST,
                    core::EventPtr(new core::Event()));
	m_found = false;
    }


    if (m_found)
    {
        // Stort pipes by X
        std::sort(pipes.begin(), pipes.end(), &BarbedWireDetector::pipeYSorter);
        
        // Get the top pipe and its width 
        Pipe topPipe = pipes[0];
        double topWidth = ((double)topPipe.getWidth()) /
            ((double)input->getWidth());

        // Create the barbed wire event, and assume there is no bottom pipe
        BarbedWireEventPtr event(new BarbedWireEvent(topPipe.getX(),
                                                     topPipe.getY(),
                                                     topWidth, 0, 0, -1));

        // Now update the information if there is a bottom pipe
        if (pipes.size() > 1)
        {
            Pipe bottomPipe = pipes[1];
            event->bottomX = bottomPipe.getX();
            event->bottomY = bottomPipe.getY();
            event->bottomWidth =
                ((double)bottomPipe.getWidth()) / input->getWidth();
        }

        // Notify every that we have found the barbed wired
        publish(EventType::BARBED_WIRE_FOUND, event);
    }

    if (output && m_found && pipes.size() > 1)
    {
        // Draw a line which connects our two pipes
        CvPoint startPt;
        startPt.x = pipes[0].getCenterX();
        startPt.y = pipes[0].getCenterY();
        
        CvPoint endPt;
        endPt.x = pipes[1].getCenterX();
        endPt.y = pipes[1].getCenterY();
        
        cvLine(output->asIplImage(), startPt, endPt, CV_RGB(255,0,0), 3, CV_AA,
               0);
    }
    

    /// TODO: consider detection stragies for side on blob
    /// TODO: do some blob merging as needed
}

void BarbedWireDetector::filterForGreen(Image* input)
{
    // Remove top and bottom chunks if desired
    if (m_topRemovePercentage != 0)
    {
        int linesToRemove = (int)(m_topRemovePercentage * m_image->getHeight());
        size_t bytesToBlack = linesToRemove * m_image->getWidth() * 3;
        memset(m_image->getData(), 0, bytesToBlack);
    }

    if (m_bottomRemovePercentage != 0)
    {
        int linesToRemove = 
	    (int)(m_bottomRemovePercentage * m_image->getHeight());
        size_t bytesToBlack = linesToRemove * m_image->getWidth() * 3;
	int startIdx = 
            m_image->getWidth() * m_image->getHeight() * 3 - bytesToBlack;
        memset(&(m_image->getData()[startIdx]), 0, bytesToBlack);
    }

    // Filter the image so all green is white, and everything else is black
    m_filter->filterImage(m_image);

    if (m_erodeIterations)
    {
      cvErode(m_image->asIplImage(), m_image->asIplImage(), 0, 
	      m_erodeIterations);
    }
    if (m_dilateIterations)
    {
      cvDilate(m_image->asIplImage(), m_image->asIplImage(), 0, 
	       m_dilateIterations);
    }

}
    
bool BarbedWireDetector::processPipes(const PipeDetector::PipeList& pipes,
                                      PipeDetector::PipeList& outPipes)
{
    bool foundPipes = false;
    
    //Assumed sorted biggest to smallest by PipeDetector.
    BOOST_FOREACH(PipeDetector::Pipe pipe, getPipes())
    {
        // Determine if we have a valid green pipe
        if (fabs(pipe.getAngle().valueDegrees()) > m_minAngle)
        {
            outPipes.push_back(pipe);
            foundPipes = true;
        }
    }

    return foundPipes;
}

bool BarbedWireDetector::pipeYSorter(PipeDetector::Pipe pipe1,
                                     PipeDetector::Pipe pipe2)
{
    return pipe1.getY() > pipe2.getY();
}
        
} // namespace vision
} // namespace ram

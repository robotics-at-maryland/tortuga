/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/OrangePipeDetector.cpp
 */

// STD Includes
#include <iostream>
#include <algorithm>
#include <string>

// Library Includes
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include <boost/foreach.hpp>
#include <boost/bind.hpp>

// Project Includes
#include "vision/include/main.h"
#include "vision/include/OrangePipeDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Camera.h"
#include "vision/include/Events.h"
#include "vision/include/ImageFilter.h"
#include "vision/include/ColorFilter.h"
#include "vision/include/TableColorFilter.h"

#include "math/include/Vector2.h"

#include "core/include/PropertySet.h"

using namespace std;

namespace ram {
namespace vision {

static bool pipeToCenterComparer(PipeDetector::Pipe b1, PipeDetector::Pipe b2)
{
    return b1.distanceTo(0,0) < b2.distanceTo(0,0);
}
    
OrangePipeDetector::OrangePipeDetector(core::ConfigNode config,
                                       core::EventHubPtr eventHub) :
    PipeDetector(config, eventHub),
    m_centered(false),
    m_colorFilterLookupTable(false),
    m_lookupTablePath("")
{
    init(config);
}
    
void OrangePipeDetector::init(core::ConfigNode config)
{
    m_angle = math::Degree(0);
    m_lineX = 0;
    m_lineY = 0;
    m_found = false;
    m_filter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_useLUVFilter = false;
    
    // Detection variables
    // NOTE: The property set automatically loads the value from the given
    //       config if its present, if not it uses the default value presented.
    core::PropertySetPtr propSet(getPropertySet());

    propSet->addProperty(config, false, "centeredLimit",
        "Max distance from the center for the pipe to be considered centered",
        0.1, &m_centeredLimit);

    propSet->addProperty(config, false, "minBrightness",
        "Minimum brighness for orange",
        100, &m_minBrightness, 0, 255);

    propSet->addProperty(config, false, "erodeIterations",
        "How many times to erode the filtered image",
        1, &m_erodeIterations);

    propSet->addProperty(config, false, "openIterations",
                         "How many times to perform the open morphological operation",
                         0, &m_openIterations);
                         

    propSet->addProperty(config, false, "rOverGMin",
       "Red/Green minimum ratio", 1.0, &m_rOverGMin, 0.0, 5.0);
    propSet->addProperty(config, false, "rOverGMax",
        "Red/Green maximum ratio", 2.0, &m_rOverGMax, 0.0, 5.0);
    propSet->addProperty(config, false, "bOverRMax",
        "Blue/Red maximum ratio",  0.4, &m_bOverRMax, 0.0, 5.0);


    // Newer Color filter properties
    propSet->addProperty(config, false, "useLUVFilter",
        "Use LUV based color filter",  true, &m_useLUVFilter);
    
    propSet->addProperty(config, false, "ColorFilterLookupTable",
        "True uses color filter lookup table", false,
        boost::bind(&OrangePipeDetector::getLookupTable, this),
        boost::bind(&OrangePipeDetector::setLookupTable, this, _1));

    m_filter->addPropertiesToSet(propSet, &config,
                                 "L", "L*",
                                 "U", "Blue Chrominance",
                                 "V", "Red Chrominance",
                                 0, 129,  // L defaults
                                 14, 200,  // U defaults
                                 126, 255); // V defaults

    
    // Make sure the configuration is valid
    //propSet->verifyConfig(config, true);
}

bool OrangePipeDetector::getLookupTable()
{
    return m_colorFilterLookupTable;
}

void OrangePipeDetector::setLookupTable(bool lookupTable)
{
    if ( lookupTable ) {
        m_colorFilterLookupTable = true;

        // Initializing ColorFilterTable
        m_lookupTablePath =
            "/home/steven/ImageFilter/LookupTables/doubleRedBuoyBlend1.25.serial";
        m_tableColorFilter = new TableColorFilter(m_lookupTablePath);
    } else {
        m_colorFilterLookupTable = false;
    }
}

void OrangePipeDetector::filterForOrangeOld(Image* image)
{
    if (m_found)
    {
        mask_orange(image->asIplImage(),true, m_minBrightness, true,
		    m_rOverGMin, 
		    m_rOverGMax, 
		    m_bOverRMax);
    }
    else
    {
        mask_orange(image->asIplImage(),true, m_minBrightness, false,
		    m_rOverGMin, 
		    m_rOverGMax, 
		    m_bOverRMax);
    }
}

void OrangePipeDetector::filterForOrangeNew(Image* image)
{
    // Filter the image so all orange is white, and everything else is black
    image->setPixelFormat(Image::PF_LUV_8);
    if ( m_colorFilterLookupTable )
        m_tableColorFilter->filterImage(image);
    else
        m_filter->filterImage(image);
}
    
bool OrangePipeDetector::found()
{
    return m_found;
}

double OrangePipeDetector::getX()
{
    return m_lineX;
}

double OrangePipeDetector::getY()
{
    return m_lineY;
}

math::Degree OrangePipeDetector::getAngle()
{
    return m_angle;
}

void OrangePipeDetector::setUseLUVFilter(bool value)
{
    m_useLUVFilter = value;
}   

OrangePipeDetector::~OrangePipeDetector()
{
    delete m_filter;

    if ( m_colorFilterLookupTable )
        delete m_tableColorFilter;
}

void OrangePipeDetector::processImage(Image* input, Image* output)
{
    //Plan is:  Search out orange with a strict orange filter, as soon as we
    // see a good deal of orange use a less strict filter, and reduce the
    // amount we need to see. In theory this makes us follow the pipeline as
    // long as possible, but be reluctant to follow some arbitrary blob until
    // we know its of a large size and the right shade of orange.  If the
    // pipeline is found, the angle found by hough is reported.  
    
    // Mask orange takes frame, then alter image, then strictness (true=more

    input->setPixelFormat(Image::PF_BGR_8);
    
    // Filter the image for the proper color
    if (m_useLUVFilter)
        filterForOrangeNew(input);
    else
        filterForOrangeOld(input);

    // 3 x 3 default erosion element, default 3 iterations.
    cvErode(input->asIplImage(), input->asIplImage(), 0, m_erodeIterations);

    if(m_openIterations > 0)
    {
        cvErode(input->asIplImage(), input->asIplImage(), 0, m_openIterations);
        cvDilate(input->asIplImage(), input->asIplImage(), 0, m_openIterations);
    }

    // Debug display
    if (output)
        output->copyFrom(input);

    // Find all of our pipes
    PipeDetector::processImage(input, output);
    PipeDetector::PipeList pipes = getPipes();

    // Determine if we found any pipes
    bool found = pipes.size() > 0;
    
    // Determine which pipes (by id) were present last time, but aren't present
    // now.  Also build the set of current pipe IDs

    // Get the set of the Ids of the newest pipes
    std::set<int> newIds;
    BOOST_FOREACH(PipeDetector::Pipe pipe, pipes)
        newIds.insert(pipe.getId());

    // Get the Set Ids that were in the last frame but not the current
    std::vector<int> lostIds(m_lastPipeIds.size()); 
    std::vector<int>::iterator lostIdsEnd =
        std::set_difference(m_lastPipeIds.begin(), m_lastPipeIds.end(),
                            newIds.begin(), newIds.end(), lostIds.begin());
    lostIds.resize(lostIdsEnd -lostIds.begin());
    
    // Send out lost events for all the pipes we lost
    BOOST_FOREACH(int id, lostIds)
    {
        PipeEventPtr event(new PipeEvent(0, 0, 0, 0));
        event->id = id;
        publish(EventType::PIPE_DROPPED, event);
    }
    
    // Send out found events for all the pipes we currently see
    BOOST_FOREACH(PipeDetector::Pipe pipe, pipes)
    {
        PipeEventPtr event(new PipeEvent(0, 0, 0, 0));
        event->id = pipe.getId();
        event->x = pipe.getX();
        event->y = pipe.getY();
        event->angle = pipe.getAngle();
        publish(EventType::PIPE_FOUND, event);
    }

    // Record the current set of Ids
    m_lastPipeIds = newIds;

    // Send the lost event if we lost all the pipes
    if (!found && m_found)
    {
        // We have lost the pipe
        publish(EventType::PIPE_LOST, core::EventPtr(new core::Event()));
    }
    m_found = found;
    
    if (m_found)
    {
        // Record the center pipe information
        m_lineX = pipes[0].getX();
        m_lineY = pipes[0].getY();
        m_angle = pipes[0].getAngle();
        
        // Send out the centerted event
        std::sort(pipes.begin(), pipes.end(), pipeToCenterComparer);

        // Determine Centered
        math::Vector2 toCenter(pipes[0].getX(), pipes[0].getY());
        if (toCenter.normalise() < m_centeredLimit)
        {
            if(!m_centered)
            {
                PipeEventPtr event(new PipeEvent(0, 0, 0, 0));
                event->x = pipes[0].getX();
                event->y = pipes[0].getY();
                event->angle = pipes[0].getAngle();
                event->id = pipes[0].getId();

                m_centered = true;
                publish(EventType::PIPE_CENTERED, event);
            }
        }
        else
        {
            m_centered = false;
        }
    }
    
    //    if (output)
    //    {
    //        CvPoint center;
    //	center.x = linex;
    //	center.y = liney;
    //	cvCircle(output->asIplImage(), center, 5, CV_RGB(0, 0, 255), -1);
    //    }
}

} // namespace vision
} // namespace ram

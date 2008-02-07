/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim <dhakim@umd.edu>
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/Events.h
 */

#ifndef RAM_VISION_EVENTS_01_08_2008
#define RAM_VISION_EVENTS_01_08_2008

// Project Includes
#include "core/include/Event.h"
#include "vision/include/Common.h"

namespace ram {
namespace vision {

class EventType 
{
    public:
    static const core::Event::EventType LIGHT_FOUND;
    static const core::Event::EventType PIPE_FOUND;
    static const core::Event::EventType GATE_FOUND;
    static const core::Event::EventType BIN_FOUND;
};

class ImageEvent : public core::Event
{
public:
    ImageEvent(Image* image_)
        {image = image_;}

    Image* image;
};

typedef boost::shared_ptr<ImageEvent> ImageEventPtr;
    
class RedLightEvent : public core::Event
{
public:
    RedLightEvent(double x, double y)
        {this->x=x; this->y=y;}
    
    double x;
    double y;
    int pixCount;
};

typedef boost::shared_ptr<RedLightEvent> RedLightEventPtr;
    
class PipeEvent : public core::Event
{
public:
    PipeEvent(double x, double y, double angle)
        { this->x=x; this->y=y; this->angle=angle;}
    
    double x;
    double y;
    double angle;
};

typedef boost::shared_ptr<PipeEvent> PipeEventPtr;
    
} // namespace vision
} // namespace ram

#endif // RAM_VISION_EVENTS_01_08_2008

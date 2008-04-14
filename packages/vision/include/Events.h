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
#include "vision/include/Common.h"
#include "core/include/Event.h"
#include "math/include/Math.h"

// Must Be Included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

struct RAM_EXPORT EventType 
{
    static const core::Event::EventType LIGHT_FOUND;
    static const core::Event::EventType LIGHT_LOST;
    static const core::Event::EventType LIGHT_ALMOST_HIT;
    static const core::Event::EventType PIPE_FOUND;
    static const core::Event::EventType PIPE_CENTERED;
    static const core::Event::EventType PIPE_LOST;
    static const core::Event::EventType GATE_FOUND;
    static const core::Event::EventType BIN_FOUND;
};

class RAM_EXPORT ImageEvent : public core::Event
{
public:
    ImageEvent(Image* image_)
        {image = image_;}

    Image* image;
};

typedef boost::shared_ptr<ImageEvent> ImageEventPtr;
    
class RAM_EXPORT RedLightEvent : public core::Event
{
public:
    RedLightEvent(double x, double y)
        {this->x=x; this->y=y;}

    math::Degree azimuth;
    math::Degree elevation;
    double range;
    double x;
    double y;
    int pixCount;
};

typedef boost::shared_ptr<RedLightEvent> RedLightEventPtr;
    
class RAM_EXPORT PipeEvent : public core::Event
{
public:
    PipeEvent(double x, double y, double angle)
        { this->x=x; this->y=y; this->angle=angle;}
    
    double x;
    double y;
    math::Degree angle;
};

typedef boost::shared_ptr<PipeEvent> PipeEventPtr;
    
} // namespace vision
} // namespace ram

#endif // RAM_VISION_EVENTS_01_08_2008

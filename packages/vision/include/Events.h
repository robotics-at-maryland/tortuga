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
#include "vision/include/Suit.h"

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
    static const core::Event::EventType BIN_LOST;
    static const core::Event::EventType BIN_DROPPED;
    static const core::Event::EventType BIN_CENTERED;
    static const core::Event::EventType MULTI_BIN_ANGLE;
    static const core::Event::EventType DUCT_FOUND;
    static const core::Event::EventType DUCT_LOST;
    static const core::Event::EventType DOWN_DUCT_FOUND;
    static const core::Event::EventType DOWN_DUCT_LOST;
    static const core::Event::EventType SAFE_FOUND;
    static const core::Event::EventType SAFE_LOST;
    static const core::Event::EventType TARGET_FOUND;
    static const core::Event::EventType TARGET_LOST;
    static const core::Event::EventType BARBED_WIRE_FOUND;
    static const core::Event::EventType BARBED_WIRE_LOST;
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

class RAM_EXPORT BinEvent : public core::Event
{
public:
//    BinEvent(double x, double y)
//    { this->x=x; this->y=y; this->suit = Suit::NONEFOUND;}
    BinEvent(double x, double y, Suit::SuitType s, math::Degree angle)
    { this->x=x; this->y=y; this->suit = s; this->angle = angle;}
    
    BinEvent(math::Degree angle)
    {this->x = 0; this->y = 0; this->suit = Suit::UNKNOWN; this->angle = angle;}
    
    double x;
    double y;
    int id;
    Suit::SuitType suit;
    math::Degree angle;
};

typedef boost::shared_ptr<BinEvent> BinEventPtr;

class RAM_EXPORT DuctEvent : public core::Event
{
public:
    DuctEvent(double x, double y, double range, 
        double rotation, bool aligned, bool visible) :
        x(x), y(y), range(range), alignment(rotation),
        aligned(aligned), visible(visible)
        {
        }
    
    double x;
    double y;
    double range;
    double alignment;
    bool aligned;
    bool visible;
};

typedef boost::shared_ptr<DuctEvent> DuctEventPtr;

class RAM_EXPORT SafeEvent : public core::Event
{
  public:
    SafeEvent(double centerX, double centerY)
    {
        x = centerX;
        y = centerY;
    }
    double x;
    double y;
};
    
typedef boost::shared_ptr<SafeEvent> SafeEventPtr;

class RAM_EXPORT TargetEvent : public core::Event
{
  public:
    TargetEvent(double centerX, double centerY, double squareNess_,
                double range_) :
        x(centerX),
        y(centerY),
        squareNess(squareNess_),
        range(range_)
    {
    }
    double x;
    double y;
    double squareNess;
    double range;
};
    
typedef boost::shared_ptr<TargetEvent> TargetEventPtr;

class RAM_EXPORT BarbedWireEvent : public core::Event
{
  public:
    BarbedWireEvent(double topX_, double topY_, double topWidth_,
                    double bottomX_, double bottomY_, double bottomWidth_) :
        topX(topX_),
        topY(topY_),
        topWidth(topWidth),
        bottomX(bottomX_),
        bottomY(bottomY_),
        bottomWidth(bottomWidth)
    {
    }
    double topX;
    double topY;
    double topWidth;
    double bottomX;
    double bottomY;
    double bottomWidth;
};
    
typedef boost::shared_ptr<BarbedWireEvent> BarbedWireEventPtr;

    
} // namespace vision
} // namespace ram

#endif // RAM_VISION_EVENTS_01_08_2008

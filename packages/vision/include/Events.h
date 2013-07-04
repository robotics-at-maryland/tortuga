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
#include "vision/include/Symbol.h"
#include "vision/include/Color.h"

#include "core/include/Event.h"

#include "math/include/Math.h"

// Must Be Included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

struct RAM_EXPORT EventType 
{
    static const core::Event::EventType BUOY_FOUND;
    static const core::Event::EventType BUOY_DROPPED;
    static const core::Event::EventType BUOY_LOST;
    static const core::Event::EventType BUOY_ALMOST_HIT;
    static const core::Event::EventType BUOY_DETECTOR_ON;
    static const core::Event::EventType BUOY_DETECTOR_OFF;

    static const core::Event::EventType PIPE_FOUND;
    static const core::Event::EventType PIPE_CENTERED;
    static const core::Event::EventType PIPE_LOST;
    static const core::Event::EventType PIPE_DROPPED;
    static const core::Event::EventType PIPELINE_DETECTOR_ON;
    static const core::Event::EventType PIPELINE_DETECTOR_OFF;

    static const core::Event::EventType GATE_FOUND;
    static const core::Event::EventType GATE_DETECTOR_ON;
    static const core::Event::EventType GATE_DETECTOR_OFF;

    static const core::Event::EventType BIN_FOUND;
    static const core::Event::EventType BINS_LOST;
    static const core::Event::EventType BIN_DROPPED;
    static const core::Event::EventType BIN_CENTERED;
    static const core::Event::EventType BIN_DETECTOR_ON;
    static const core::Event::EventType BIN_DETECTOR_OFF;
    static const core::Event::EventType MULTI_BIN_ANGLE;

    static const core::Event::EventType LOVERSLANE_FOUND;
    static const core::Event::EventType LOVERSLANE_DROPPED;
    static const core::Event::EventType LOVERSLANE_LOST;
    static const core::Event::EventType LOVERSLANE_DETECTOR_ON;
    static const core::Event::EventType LOVERSLANE_DETECTOR_OFF;

    static const core::Event::EventType CAESAR_DETECTOR_ON;
    static const core::Event::EventType CAESAR_DETECTOR_OFF;
    static const core::Event::EventType CAESAR_FOUND;
    static const core::Event::EventType CAESAR_LOST;
    static const core::Event::EventType CAESAR_SMALL_FOUND;
    static const core::Event::EventType CAESAR_SMALL_LOST;
    static const core::Event::EventType CAESAR_LARGE_FOUND;
    static const core::Event::EventType CAESAR_LARGE_LOST;


    //----- OLD EVENTS - REMOVING WILL BREAK CODE ------------
    static const core::Event::EventType CUPID_DETECTOR_ON;
    static const core::Event::EventType CUPID_DETECTOR_OFF;
    static const core::Event::EventType CUPID_FOUND;
    static const core::Event::EventType CUPID_LOST;
    static const core::Event::EventType CUPID_SMALL_FOUND;
    static const core::Event::EventType CUPID_SMALL_LOST;
    static const core::Event::EventType CUPID_LARGE_FOUND;
    static const core::Event::EventType CUPID_LARGE_LOST;

    static const core::Event::EventType TARGET_FOUND;
    static const core::Event::EventType TARGET_LOST;
    static const core::Event::EventType TARGET_DETECTOR_ON;
    static const core::Event::EventType TARGET_DETECTOR_OFF;
    static const core::Event::EventType WINDOW_FOUND;
    static const core::Event::EventType WINDOW_LOST;
    static const core::Event::EventType WINDOW_DETECTOR_ON;
    static const core::Event::EventType WINDOW_DETECTOR_OFF;
    static const core::Event::EventType BARBED_WIRE_FOUND;
    static const core::Event::EventType BARBED_WIRE_LOST;
    static const core::Event::EventType BARBED_WIRE_DETECTOR_ON;
    static const core::Event::EventType BARBED_WIRE_DETECTOR_OFF;
    static const core::Event::EventType HEDGE_FOUND;
    static const core::Event::EventType HEDGE_LOST;
    static const core::Event::EventType HEDGE_DETECTOR_ON;
    static const core::Event::EventType HEDGE_DETECTOR_OFF;
    static const core::Event::EventType VELOCITY_UPDATE;
    static const core::Event::EventType RED_LIGHT_DETECTOR_ON;
    static const core::Event::EventType RED_LIGHT_DETECTOR_OFF;
    static const core::Event::EventType LIGHT_FOUND;
    static const core::Event::EventType LIGHT_LOST;
    static const core::Event::EventType LIGHT_ALMOST_HIT;
    static const core::Event::EventType SAFE_FOUND;
    static const core::Event::EventType SAFE_LOST;
    static const core::Event::EventType SAFE_DETECTOR_ON;
    static const core::Event::EventType SAFE_DETECTOR_OFF;
    static const core::Event::EventType DUCT_FOUND;
    static const core::Event::EventType DUCT_LOST;
    static const core::Event::EventType DUCT_DETECTOR_ON;
    static const core::Event::EventType DUCT_DETECTOR_OFF;
    static const core::Event::EventType DOWN_DUCT_FOUND;
    static const core::Event::EventType DOWN_DUCT_LOST;
};

class RAM_EXPORT ImageEvent : public core::Event
{
public:
    ImageEvent(Image* image_)
        {image = image_;}

    ImageEvent() : image(0) {}

    Image* image;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<ImageEvent> ImageEventPtr;

class RAM_EXPORT VisionEvent : public core::Event
{
  public:
    VisionEvent(double x_, double y_, double range_) :
        x(x_), y(y_), range(range_), id(0) {}

    VisionEvent() : x(0), y(0), range(0), id(0) {}

    double x;
    double y;
    double range;
    int id;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<VisionEvent> VisionEventPtr;

class RAM_EXPORT BuoyEvent : public VisionEvent
{
public:
    BuoyEvent() : VisionEvent(), color(Color::UNKNOWN) {};
    BuoyEvent(double x_, double y_, double range_, 
              math::Degree azimuth_, math::Degree elevation_,
              Color::ColorType color_, bool touchingEdge_) :
        VisionEvent(x_, y_, range_),
        azimuth(azimuth_),
        elevation(elevation_),
        color(color_),
        touchingEdge(touchingEdge_) {}

    math::Degree azimuth;
    math::Degree elevation;
    Color::ColorType color;
    bool touchingEdge;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<BuoyEvent> BuoyEventPtr;

class RAM_EXPORT CupidEvent : public VisionEvent
{
public:
    CupidEvent() : VisionEvent(), color(Color::UNKNOWN) {};
    CupidEvent(double x_, double y_, double range_, 
               math::Degree azimuth_, math::Degree elevation_,
               Color::ColorType color_, bool touchingEdge_) :
        VisionEvent(x_, y_, range_),
        azimuth(azimuth_),
        elevation(elevation_),
        color(color_),
        touchingEdge(touchingEdge_) {}

    math::Degree azimuth;
    math::Degree elevation;
    Color::ColorType color;
    bool touchingEdge;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<CupidEvent> CupidEventPtr;

class RAM_EXPORT CaesarEvent : public VisionEvent
{
public:
    CaesarEvent() : VisionEvent(), color(Color::UNKNOWN) {};
    CaesarEvent(double x_, double y_, double range_, 
               math::Degree azimuth_, math::Degree elevation_,
               Color::ColorType color_, bool touchingEdge_) :
        VisionEvent(x_, y_, range_),
        azimuth(azimuth_),
        elevation(elevation_),
        color(color_),
        touchingEdge(touchingEdge_) {}

    math::Degree azimuth;
    math::Degree elevation;
    Color::ColorType color;
    bool touchingEdge;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<CaesarEvent> CaesarEventPtr;

class RAM_EXPORT PipeEvent : public VisionEvent
{
public:
    PipeEvent(double x_, double y_, double range_, double angle_) :
        VisionEvent(x_, y_, range_),
        angle(angle_) {}

    PipeEvent() : VisionEvent(), angle(math::Degree(0)) {}

    math::Degree angle;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<PipeEvent> PipeEventPtr;

class RAM_EXPORT BinEvent : public VisionEvent
{
public:
    BinEvent(double x_, double y_, double range_, 
             Symbol::SymbolType s_, math::Degree angle_) :
        VisionEvent(x_, y_, range_),
        symbol(s_), angle(angle_) {}
    
    BinEvent(math::Degree angle_) :
        VisionEvent(),
        symbol(Symbol::UNKNOWN), angle(angle_) {}

    BinEvent() : VisionEvent(), symbol(Symbol::UNKNOWN), angle(0) {}
    
    Symbol::SymbolType symbol;
    math::Degree angle;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<BinEvent> BinEventPtr;


class RAM_EXPORT LoversLaneEvent : public core::Event
{
  public:
    LoversLaneEvent(double leftX_, double leftY_, double rightX_, double rightY_,
                    double centerX_, double centerY_,
                    double width_, double range_,
                    bool haveLeft_, bool haveRight_) :
        leftX(leftX_),
        leftY(leftY_),
        rightX(rightX_),
        rightY(rightY_),
        centerX(centerX_),
        centerY(centerY_),
        squareNess(width_),
        range(range_),
        haveLeft(haveLeft_),
        haveRight(haveRight_)
    {
    }

    LoversLaneEvent() :
        leftX(0),
        leftY(0),
        rightX(0),
        rightY(0),
        centerX(0),
        centerY(0),
        squareNess(0),
        range(0),
        haveLeft(false),
        haveRight(false)
    {
    }

    double leftX;
    double leftY;
    double rightX;
    double rightY;
    double centerX;
    double centerY;

    // Calculated on width / height
    double squareNess;
    double range;

    bool haveLeft;
    bool haveRight;


    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<LoversLaneEvent> LoversLaneEventPtr;








class RAM_EXPORT HedgeEvent : public core::Event
{
  public:
    HedgeEvent(double leftX_, double leftY_, double rightX_, double rightY_,
               double width_, double range_,
               bool haveLeft_, bool haveRight_) :
        leftX(leftX_),
        leftY(leftY_),
        rightX(rightX_),
        rightY(rightY_),
        squareNess(width_),
        range(range_),
        haveLeft(haveLeft_),
        haveRight(haveRight_)
    {
    }

    HedgeEvent() :
        leftX(0),
        leftY(0),
        rightX(0),
        rightY(0),
        squareNess(0),
        range(0),
        haveLeft(false),
        haveRight(false)
    {
    }

    double leftX;
    double leftY;
    double rightX;
    double rightY;

    // Calculated on width / height
    double squareNess;
    double range;

    bool haveLeft;
    bool haveRight;


    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<HedgeEvent> HedgeEventPtr;









class RAM_EXPORT GateEvent : public core::Event
{
  public:
    GateEvent(double leftX_, double leftY_, double rightX_, double rightY_,
               double width_, double range_,
               bool haveLeft_, bool haveRight_) :
        leftX(leftX_),
        leftY(leftY_),
        rightX(rightX_),
        rightY(rightY_),
        squareNess(width_),
        range(range_),
        haveLeft(haveLeft_),
        haveRight(haveRight_)
    {
    }

    GateEvent() :
        leftX(0),
        leftY(0),
        rightX(0),
        rightY(0),
        squareNess(0),
        range(0),
        haveLeft(false),
        haveRight(false)
    {
    }

    double leftX;
    double leftY;
    double rightX;
    double rightY;

    // Calculated on width / height
    double squareNess;
    double range;

    bool haveLeft;
    bool haveRight;


    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<GateEvent> GateEventPtr;







class RAM_EXPORT RedLightEvent : public core::Event
{
public:
    RedLightEvent(double x_, double y_,
                  Color::ColorType color_=Color::UNKNOWN) :
        azimuth(0),
        elevation(0),
        range(0),
        x(x_),
        y(y_),
        pixCount(0),
        color(color_)
    {
    }
    
    RedLightEvent() : azimuth(0), elevation(0), range(0), x(0), y(0),
        pixCount(0), color(Color::UNKNOWN) {}

    math::Degree azimuth;
    math::Degree elevation;
    double range;
    double x;
    double y;
    int pixCount;
    Color::ColorType color;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<RedLightEvent> RedLightEventPtr;


class RAM_EXPORT BarbedWireEvent : public core::Event
{
  public:
    BarbedWireEvent(double topX_, double topY_, double topWidth_,
                    double bottomX_, double bottomY_, double bottomWidth_) :
        topX(topX_),
        topY(topY_),
        topWidth(topWidth_),
        bottomX(bottomX_),
        bottomY(bottomY_),
        bottomWidth(bottomWidth_)
    {
    }

    BarbedWireEvent() :
        topX(0),
        topY(0),
        topWidth(0),
        bottomX(0),
        bottomY(0),
        bottomWidth(0)
    {
    }


    double topX;
    double topY;
    double topWidth;
    double bottomX;
    double bottomY;
    double bottomWidth;

    virtual core::EventPtr clone();
};
    
typedef boost::shared_ptr<BarbedWireEvent> BarbedWireEventPtr;

//Kate added small and large centers, since each panel has a small and large target hole
class RAM_EXPORT TargetEvent : public core::Event
{
  public:
    TargetEvent(double centerX, double centerY, double largeCenterX, double largeCenterY, double smallCenterX, double smallCenterY,double squareNess_,
                double range_, Color::ColorType color_ = Color::UNKNOWN) :
        x(centerX),
        y(centerY),
	largex(largeCenterX),
        largey(largeCenterY),
	smallx(smallCenterX),
        smally(smallCenterY),
        squareNess(squareNess_),
        range(range_),
        color(color_)
    {
    }

    TargetEvent() :
        x(0),
        y(0),
	largex(0),
	largey(0),
	smallx(0),
	smally(0),
        squareNess(0),
        range(0),
        color(Color::UNKNOWN)
    {
    }


    double x;
    double y;
    double largex; 
    double largey;
    double smallx;
    double smally;
    double panelx;
    double panely;
    double squareNess;
    double range;
    Color::ColorType color;

    virtual core::EventPtr clone();
};
    
typedef boost::shared_ptr<TargetEvent> TargetEventPtr;
typedef TargetEvent WindowEvent;
typedef TargetEventPtr WindowEventPtr;

class RAM_EXPORT SafeEvent : public core::Event
{
  public:
    SafeEvent(double centerX, double centerY) : x(centerX), y(centerY)
    {}
    
    SafeEvent() : x(0), y(0)
    {}

    double x;
    double y;

    virtual core::EventPtr clone();
};
    
typedef boost::shared_ptr<SafeEvent> SafeEventPtr;


class RAM_EXPORT DuctEvent : public core::Event
{
public:
    DuctEvent(double x, double y, double range, 
        double rotation, bool aligned, bool visible) :
        x(x), y(y), range(range), alignment(rotation),
        aligned(aligned), visible(visible)
        {
        }

    DuctEvent() :
        x(0), y(0), range(0), alignment(0),
        aligned(0), visible(0)
    {}

    
    double x;
    double y;
    double range;
    double alignment;
    bool aligned;
    bool visible;

    virtual core::EventPtr clone();
};

typedef boost::shared_ptr<DuctEvent> DuctEventPtr;


} // namespace vision
} // namespace ram

#endif // RAM_VISION_EVENTS_01_08_2008

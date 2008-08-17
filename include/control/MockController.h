#ifndef _MOCKCONTROLLER_H
#define _MOCKCONTROLLER_H

#include <ram.h>
#include <iostream>

class MockController : public ram::control::IController
{
public:
    virtual void setSpeed(double speed_, const Ice::Current&)
	{
		std::cout << "Setting speed to " << speed_ << std::endl;
		speed = speed_;
	}

    virtual void setDepth(double depth_, const Ice::Current&) { depth = depth_; }

    virtual double getSpeed(const Ice::Current&)
	{
		std::cout << "Getting speed (" << speed << ")" << std::endl;
		return speed;
	}

    virtual double getDepth(const Ice::Current&) { return depth; }
    
    virtual double getEstimatedDepth(const Ice::Current&) { return 0; }
    
    virtual double getEstimatedDepthDot(const Ice::Current&) { return 0; }

    virtual void yawVehicle(double degrees, const Ice::Current&) { yaw = degrees; }

    virtual void pitchVehicle(double degrees, const Ice::Current&) {  }

    virtual void rollVehicle(double degrees, const Ice::Current&) { }

    virtual struct ram::math::Quaternion getDesiredOrientation(const Ice::Current&)
	{ ram::math::Quaternion q;return q; }
    
    virtual bool atOrientation(const Ice::Current&) { return atorientation; }
    
    virtual bool atDepth(const Ice::Current&) { return atdepth; }


    virtual void update(double, const Ice::Current&) {}
    virtual void background(int, const Ice::Current&) {}
    virtual void unbackground(bool, const Ice::Current&) {}
    virtual bool backgrounded(const Ice::Current&) { return false; }
    
private:
    double speed;
    double depth;
    double yaw;
    bool atorientation;
    bool atdepth;
};

#endif

/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/vision/src/Camera.cpp
 */


// Library Includes
#include <boost/python.hpp>

// Project Includes
#include "vision/include/Camera.h"
#include "vision/include/Image.h"

namespace bp = boost::python;

class CameraWrapper : public ram::vision::Camera,
                      public bp::wrapper< ram::vision::Camera >
{
public:
    CameraWrapper(int width, int height) :
        m_width(width),
        m_height(height),
        _backgrounded(false)
    {
    }

    ~CameraWrapper()
    {
        cleanup();
    }

    void capturedImage(ram::vision::Image* image)
    {
        m_width = image->getWidth();
        m_height = image->getHeight();

        ram::vision::Camera::capturedImage(image);
    }
   
    virtual void update(double timestep)
    {
        // do nothing
    };

    /** Width of captured image */
    virtual size_t width()
    {
        return m_width;
    }

    /** Height of captured image */
    virtual size_t height()
    {
        return m_height;
    }

    virtual double fps()
    {
        return 30;
    }

    virtual double duration()
    {
        return 0; 
    }

    virtual void seekToTime(double seconds)
    {
    }

    virtual double currentTime()
    {
        return 0;
    }

    // Override these to be compatible with users
    virtual bool backgrounded()
    {
        return _backgrounded;
    }
    
    virtual void background(int)
    {
        _backgrounded = true;
    }

    virtual void unbackground(bool join = false)
    {
        _backgrounded = false;
        ram::vision::Camera::unbackground(join);
    };
    
private:
    int m_width;
    int m_height;
    bool _backgrounded;
};

void registerCameraClass()
{
    bp::class_<CameraWrapper, boost::noncopyable >(
        "Camera", bp::init<int, int>((bp::arg("width"), bp::arg("height"))) )
        .def("capturedImage", &CameraWrapper::capturedImage)
        .def("background", &CameraWrapper::background);
    bp::register_ptr_to_python< ram::vision::CameraPtr >();
//    bp::implicitly_convertible< ram::core::SubsystemPtr, boost::shared_ptr< ram::core::EventPublisher > >();
}

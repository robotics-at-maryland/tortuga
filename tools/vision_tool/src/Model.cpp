/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  tools/vision_viewer/include/Model.h
 */

// Library Includes
#include <wx/timer.h>

// Core Includes
#include "Model.h"

#include "core/include/EventPublisher.h"
#include "vision/include/Camera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/OpenCVCamera.h"
#include "vision/include/FFMPEGCamera.h"

RAM_CORE_EVENT_TYPE(ram::tools::visionvwr::Model, IMAGE_SOURCE_CHANGED);
RAM_CORE_EVENT_TYPE(ram::tools::visionvwr::Model, NEW_IMAGE);

namespace ram {
namespace tools {
namespace visionvwr {

BEGIN_EVENT_TABLE(Model, wxEvtHandler)
END_EVENT_TABLE()
    
Model::Model() :
    m_camera(0),
    m_timer(new wxTimer(this)),
    m_newImage(new vision::OpenCVImage(640, 480))
{
}

Model::~Model()
{
}
    
void Model::openFile(std::string filename)
{
    if (m_camera)
        delete m_camera;
        
    m_camera = new vision::FFMPEGCamera(filename);
    sendImageSourceChanged();
}
    
void Model::openCamera(int num)
{
    if (m_camera)
        delete m_camera;

    if (-1 == num)
        m_camera = new vision::OpenCVCamera();
    else
        m_camera = new vision::OpenCVCamera(num, true);
    sendImageSourceChanged();
}


void Model::stop()
{
    m_timer->Stop();
}

void Model::start()
{
    double fpsNum = fps();
    if (fpsNum == 0.0)
        fpsNum = 30;
    m_timer->Start((int)(1000 / fpsNum));
}

bool Model::running()
{
    return m_timer->IsRunning();
}

double Model::fps()
{
    if (m_camera)
        return m_camera->fps();
    else
        return 0;
}
    
double Model::duration()
{
    if (m_camera)
        return m_camera->duration();
    else
        return 0;
}
    
void Model::seekToTime(double seconds)
{
    if (m_camera)
        m_camera->seekToTime(seconds);
}
    

double Model::currentTime()
{
    if (m_camera)
        return m_camera->currentTime();
    else
        return 0;
}

void Model::onTimer(wxTimerEvent &event)
{
    /// TODO: copy new image into m_newImage
    
    sendNewImage();
}
    
void Model::sendNewImage()
{

}
    
void Model::sendImageSourceChanged()
{

}


} // namespace visionvwr
} // namespace tools
} // namespace ram

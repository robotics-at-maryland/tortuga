/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joesph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/Recorder.cpp
 */

#ifndef RAM_RECORDER_H_06_24_2007
#define RAM_RECORDER_H_06_24_2007

#define _CRT_SECURE_NO_WARNINGS

// Library Includes
#include <boost/thread/mutex.hpp>

// Project Includes
#include "vision/include/Common.h"
#include "core/include/Forward.h"
#include "core/include/Updatable.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

/** A base class for recording images from a camera
 *
 *  This class supports several different recording policies which all you to
 *  record all the images from a camera, as many as possible, or a fixed rate
 *  of images.  To use this class just implement Recorder::recordFrame and call
 *  Recorder::cleanUp in you desctuctor.
 *
 */
class RAM_EXPORT Recorder : public core::Updatable
{
  public:
    enum RecordingPolicy
    {
        RP_START,   /** Sentinal Value */
        NEXT_FRAME, /** Record frame, wait for the next, record; repeat */
        RP_END,     /** Sentinal Value */
    };
    
    Recorder(Camera* camera, Recorder::RecordingPolicy policy);
        
    ~Recorder();

    virtual void update(double timeSinceLastUpdate);

  protected:
    /** This must be the first thing called in by a subclasses destructor */
    virtual void cleanUp();
    
    /** Called when ever there is a new frame to record */
    virtual void recordFrame(Image* image) = 0;


  private:
    /** Called when the camera has processed a new event */
    void newImageCapture(core::EventPtr event);

    core::EventConnectionPtr m_connection;
    
    /** Determines how the recorder records video from the camera */
    Recorder::RecordingPolicy m_policy;
    
    /** Protects access to m_nextFrame and m_newFrame */
    boost::mutex m_mutex;

    /** Weather or not we have a new frame */
    bool m_newFrame;
    
    /** Holds the next frame to record */
    Image* m_nextFrame;

    /** The current frame we are recording */
    Image* m_currentFrame;

    /** The camera we are recording from */
    Camera* m_camera;
};
    
} // namespace vision
} // namespace ram	

#endif // RAM_RECORDER_H_06_24_2007

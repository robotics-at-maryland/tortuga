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

// STD Includes
#include <string>

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
        MAX_RATE,   /** Don't record any faster than the given rate (in Hz) */
        NEXT_FRAME, /** Always record the next availabe frame */
        RP_END,     /** Sentinal Value */
    };

    /** Constructor
     *
     *  @param camera  The camera to record images from
     *  @param policy  Determines how often images from the camera are recorded
     *  @param policyArg  An argument for use by the given recording policy.
     *  @param recordWidth The final width of the image to record
     *  @param recordHeight The final height of the image to record
     */
    Recorder(Camera* camera, Recorder::RecordingPolicy policy,
             int policyArg = 0, int recordWidth = 640, int recordHeight = 480);
        
    ~Recorder();

    virtual void update(double timestep);

    /** Starts the background process thread, clear m_newFrame flag */
    virtual void background(int interval = -1);

    /** Width of image we are recording in pixels */
    size_t getRecordingWidth() const;

    /** Height of image we are recording in pixels */
    size_t getRecordingHeight() const;

    /** Creates a recorder from string the string
     *
     *  This can be a network recorder, file system recorder etc.
     */
    static Recorder* createRecorderFromString(const std::string& str,
                                              Camera* camera,
                                              std::string& message,
                                              Recorder::RecordingPolicy policy,
                                              int policyArg,
                                              std::string recorderDir = ".");
    
  protected:
    /** This must be the first thing called in by a subclasses destructor */
    virtual void cleanUp();

    /** Waits for the next image, only run when the recorder is backgronuded
     *
     *  By default it will block until a new image is recieved by the camera.
     */
    virtual void waitForImage(Camera* camera);
    
    /** Called when ever there is a new frame to record */
    virtual void recordFrame(Image* image) = 0;
    
  private:
    /** Called when the camera has processed a new event */
    void newImageCapture(core::EventPtr event);

    core::EventConnectionPtr m_connection;
    
    /** Determines how the recorder records video from the camera */
    Recorder::RecordingPolicy m_policy;

    /** Recording Policy specific data */
    int m_policyArg;

    /** The width of theimage we are recording in pixels */
    size_t m_width;

    /** The height of the image we are recording in pixels */
    size_t m_height;
    
    /** Protects access to m_nextFrame and m_newFrame */
    boost::mutex m_mutex;

    /** Weather or not we have a new frame */
    bool m_newFrame;
    
    /** The camera we are recording from */
    Camera* m_camera;

    /** The frame we get from the camera */
    Image* m_frameFromCamera;

    /** The current frame we are recording */
    Image* m_frameResized;

    /** Current time in seconds */
    double m_currentTime;

    /** The next time an image can be recorded */
    double m_nextRecordTime;
};
    
} // namespace vision
} // namespace ram	

#endif // RAM_RECORDER_H_06_24_2007

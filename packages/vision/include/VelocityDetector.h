/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Nate Davidge
 * All rights reserved.
 *
 * Author: Nate Davidge <ndavidge@umd.edu>
 * File:  packages/vision/include/VelocityDetector.h
 */

#ifndef RAM_VELOCITYDETECTOR_H_05_27_2009
#define RAM_VELOCITYDETECTOR_H_05_27_2009

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Detector.h"

#include "core/include/ConfigNode.h"

#include "math/include/Vector2.h"

// Must be incldued last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
    
class RAM_EXPORT VelocityDetector : public Detector
{
  public:
    VelocityDetector(core::ConfigNode config,
                     core::EventHubPtr eventHub = core::EventHubPtr());
    ~VelocityDetector();

    void update();

    void processImage(Image* input, Image* output= 0);
    
    /** Gives the motion of the camera in pixels per frame
     *
     *  @todo
     *      Make me time based, ie. pixels per second
     */
    math::Vector2 getVelocity();

    /** Sets all flags such that only phase correlation is used */
    void usePhaseCorrelation();

  private:
    void init(core::ConfigNode config);

    // Phase correlation functions and variables
    void phasePhaseCorrelation(Image* output);

    /** Current frame as grey scale */
    IplImage* m_currentGreyScale;

    /** Last frame as grey scale  */
    IplImage* m_lastGreyScale;

    /** Result of phase correlation on the image (64 bit float format) */
    IplImage* m_phaseResult;

    /** Scale the debug phase correlation velocity line */
    double m_phaseLineScale;

    // Algorithm independent members

    /** Attempt phase correlation on the image */
    bool m_usePhaseCorrelation;

    /** The frame we just recieved, compared against lastFrame */
    Image* m_currentFrame;

    /** The last frame we recieved, use to compare against */
    Image* m_lastFrame;

    /** The last velocity we calculated */
    math::Vector2 m_velocity;
};
	
} // namespace vision
} // namespace ram

#endif // RAM_VELOCITYDETECTOR_H_05_27_2009

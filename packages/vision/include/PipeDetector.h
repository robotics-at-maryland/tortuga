/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/PipeDetector.h
 */

#ifndef RAM_PIPE_DETECTOR_H_05_15_2009
#define RAM_PIPE_DETECTOR_H_05_15_2009

// STD Includes
#include <vector>

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "vision/include/BlobDetector.h"
#include "vision/include/TrackedBlob.h"
#include "core/include/ConfigNode.h"
#include "math/include/Math.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
        
class RAM_EXPORT PipeDetector  : public Detector
{
  public:
    class Pipe : public TrackedBlob
    {
    public:
        Pipe();
        Pipe(BlobDetector::Blob blob, Image* sourceImage,
             math::Degree angle, int id);

        /** Draws the bounds of the bin in green, and its ID */
        //void draw(Image* image);
    };

    typedef std::vector<Pipe> PipeList;
    typedef PipeList::iterator PipeListIter;
    
    PipeDetector(core::ConfigNode config,
                 core::EventHubPtr eventHub = core::EventHubPtr(),
                 int minPixels = 3000, int foundMinPixels = 2500);
    ~PipeDetector();

    /** Set whether or not to attempt to use the hough detector*/
    void setHough(bool value);
    
    void processImage(Image* input, Image* output= 0);

    /** True if we found any pipes */
    bool found();

    /** Returns all blobs bigger then minimum blob size, sorted large->small */
    PipeList getPipes();  
    
  private:
    void init(core::ConfigNode config,
              int minPixels, int foundMinPixels);

    /** Determines the angle of the pipe inside the given blob 
     *
     *  @param pipeBlob
     *      The blob to find the angle of
     *  @param outAngle
     *      Will be changed to reflect the pipe angle
     *  @param input
     *      Image to analyze for angle, will be changed!
     *  @param output
     *      If present debug information will be drawn
     *
     *  @todo
     *      Update this to use the hough detector first.
     *  @return
     *     True if succesfully found the angle
     */
    bool findPipeAngle(BlobDetector::Blob pipeBlob, math::Degree& outAngle,
                       Image* input, Image* output);

    /** Disabled use of hough */
    bool m_noHough;

    /** Finds the pipe blobs */
    BlobDetector m_blobDetector;

    /** All the pipes we found last frame */
    PipeList m_pipes;
    
    int m_minPixels;
    int m_minPixelsFound;
};
    
} // namespace vision
} // namespace ram

#endif // RAM_PIPE_DETECTOR_H_05_15_2009


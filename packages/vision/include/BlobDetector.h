/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/BlobDetector.h
 */

#ifndef RAM_VISION_BLOBDETECTOR_H_06_19_2008
#define RAM_VISION_BLOBDETECTOR_H_06_19_2008

// STD Includes
#include <vector>

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "core/include/ConfigNode.h"
#include "math/include/Math.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
        
class RAM_EXPORT BlobDetector  : public Detector
{
public:
    class Blob
    {
    public:
        Blob(int size, int centerX, int centerY, int maxX, int minX, int maxY,
             int minY) :
            m_size(size), m_centerX(centerX), m_centerY(centerY),
            m_maxX(maxX), m_minX(minX), m_maxY(maxY), m_minY(minY)
            {}
        
        int getSize() { return m_size; }

        int getCenterX() { return m_centerX; }
        int getCenterY() { return m_centerY; }
        
        int getMaxX() { return m_maxX; }
        int getMinX() { return m_minX; }
        
        int getMaxY() { return m_maxY; }
        int getMinY() { return m_minX; }

    private:
        int m_size;
        int m_centerX;
        int m_centerY;
        int m_maxX;
        int m_minX;
        int m_maxY;
        int m_minY;
    };
    
    BlobDetector(core::ConfigNode config,
                 core::EventHubPtr eventHub = core::EventHubPtr());
    ~BlobDetector();
    
    void processImage(Image* input, Image* output= 0);
    
    bool found();

    std::vector<Blob> getBlobs();
    
  private:
    void init(core::ConfigNode config);

    int histogram(IplImage* img);
    
    std::vector<Blob> m_blobs;
};
    
} // namespace vision
} // namespace ram

#endif // RAM_VISION_BLOBDETECTOR_H_06_19_2008


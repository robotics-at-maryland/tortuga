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

/** Finds all connected white regions in a black and white RGB image
 *
 *  As it finds all connected components it records their max and min X,Y
 *  bounds, and total white pixel counts. This is returned as a list of Blob
 *  objects ordered from largest to smallest.
 */  
class RAM_EXPORT BlobDetector  : public Detector
{
  public:

    /** A single set of connected white pixels in the image
     *
     *  Stores min/max X and Y bounds of the blob, its center, and pixel count.
     */
    class Blob
    {
    public:
        Blob(int size, int centerX, int centerY, int maxX, int minX, int maxY,
             int minY) :
            m_size(size), m_centerX(centerX), m_centerY(centerY),
            m_maxX(maxX), m_minX(minX), m_maxY(maxY), m_minY(minY) {}
        Blob():m_size(0),m_centerX(0),m_centerY(0),m_maxX(0),m_minX(0),m_maxY(0),m_minY(0) {}
        int getSize() const { return m_size; }

        // These do not return the center coordinates.  
        // They returns the average coordinate over all blob pixels
        int getCenterX() const { return m_centerX; }
        int getCenterY() const { return m_centerY; }
        
        int getMaxX() const { return m_maxX; }
        int getMinX() const { return m_minX; }
        
        int getMaxY() const { return m_maxY; }
        int getMinY() const { return m_minY; }

        int getHeight() const { return getMaxY() - getMinY() + 1; }

        int getWidth() const { return getMaxX() - getMinX() + 1; }

        /** Percentage of the area within the bounding box that is the blob */
        double getFillPercentage() { 
            return ((double)getSize()) / ((double)(getHeight() * getWidth())); } 

        /** Draws the bounds, and optionally the center of the blob */
        void draw(Image*, bool centroid = true, 
                  unsigned char R = 0,
                  unsigned char G = 0,
                  unsigned char B = 255);

        /** Draws the Aspect ratio and pixel count in text on the image */
        void drawStats(Image* image);

        /** Draws text in the upper right hand corner of the blob */
        void drawTextUL(Image* image, std::string text, int xOffset = 0,
                        int yOffset = 0);

        /** Draws text in the upper right hand corner of the blob */
        void drawTextUR(Image* image, std::string text, int xOffset = 0,
                        int yOffset = 0);
        
        /** Retuns an aspect ratio all ways greater then one */
        double getAspectRatio() const {
            double aspectRatio = getTrueAspectRatio();
            // Ensure its always positive
            if (aspectRatio < 1)
                aspectRatio = 1 / aspectRatio;
            return aspectRatio;
        }

        /** Returns an aspect ratio that goes from 0 - inf */
        double getTrueAspectRatio() const {

            double boundWidth = getHeight();
            double boundHeight = getWidth();
            return boundWidth / boundHeight;
        }

        
        bool containsExclusive(Blob otherBlob)
        {
            return (m_minX < otherBlob.m_minX &&
                    m_minY < otherBlob.m_minY &&
                    m_maxX > otherBlob.m_maxX &&
                    m_maxY > otherBlob.m_maxY);
        }
        
        bool containsInclusive(Blob otherBlob)
        {
            return (m_minX <= otherBlob.m_minX &&
                    m_minY <= otherBlob.m_minY &&
                    m_maxX >= otherBlob.m_maxX &&
                    m_maxY >= otherBlob.m_maxY);        
        }
        
        bool containsInclusive(Blob otherBlob, int addToBounds)
        {
            return (m_minX-addToBounds <= otherBlob.m_minX &&
                    m_minY-addToBounds <= otherBlob.m_minY &&
                    m_maxX+addToBounds >= otherBlob.m_maxX &&
                    m_maxY+addToBounds >= otherBlob.m_maxY); 
        }

        bool boundsIntersect(Blob otherBlob, int addToBounds = 0)
        {
            if (((m_minX - addToBounds) > otherBlob.m_maxX) || 
                ((m_maxX + addToBounds) < otherBlob.m_minX))
            {
                return false;
            }
            if (((m_minY - addToBounds) > otherBlob.m_maxY) || 
                ((m_maxY + addToBounds) < otherBlob.m_minY))
            {
                return false;
            }
            return true;
        }

    private:
        int m_size;
        int m_centerX;
        int m_centerY;
        int m_maxX;
        int m_minX;
        int m_maxY;
        int m_minY;
    };
    
    class BlobComparer
    {
    public:
        static bool compare(Blob b1, Blob b2)
        {
            return b1.getSize() > b2.getSize();
        }
    };
    
    BlobDetector(core::ConfigNode config,
                 core::EventHubPtr eventHub = core::EventHubPtr());
    BlobDetector(int minimumBlobSize = 0);
    ~BlobDetector();
    
    void processImage(Image* input, Image* output= 0);
    
    bool found();

    /** Sets the minimum number of pixels a blob must be to be reported */
    void setMinimumBlobSize(int pixels);

    /** Returns the minimum number of pixels a blob must to be reported */
    int getMinimumBlobSize();

    /** Returns all blobs bigger then minimum blob size, sorted large->small */
    std::vector<Blob> getBlobs();

    typedef std::vector<Blob> BlobList;
  private:
    /** Initializes the class */
    void init(core::ConfigNode config);

    int histogram(IplImage* img);

    /** Ensures that data array is large enough to hold desired pixel count */
    void ensureDataSize(int pixels);
    
    std::vector<Blob> m_blobs;

    /** Minimum pixel count for blobs to count */
    int m_minBlobSize;
    
    
    // Data used by internal blob algorithm
    std::vector<int> pixelCounts;
    std::vector<int> totalX;
    std::vector<int> totalY;
    std::vector<int> totalMaxX;
    std::vector<int> totalMaxY;
    std::vector<int> totalMinX;
    std::vector<int> totalMinY;
    std::vector<unsigned int> joins;

    /** "Image" used during internal processing */
    unsigned int* data;
    
    /** Number of pixels represetned in the data array */
    size_t m_dataSize;
};
    
} // namespace vision
} // namespace ram

#endif // RAM_VISION_BLOBDETECTOR_H_06_19_2008


/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vision/src/RegionOfInterest.h
 */

#ifndef RAM_VISION_REGION_OF_INTEREST
#define RAM_VISION_REGION_OF_INTEREST

// STD Includes
#include <math.h>

// Library Includes

// Project Includes

namespace ram {
namespace vision {

/** A simple abstraction of a rectangular region of interest in an image
 *
 * This is meant to help keep track of the area in an image we
 * care about so we can avoid performing computation on the parts
 * that do not matter to us.  In addition to keeping track of the
 * bounds, the regions can be tested for intersection and equality.
 * The distance between regions can be computed and as well as a
 * measure of the significance of separation between two regions.
 */

class RegionOfInterest
{
public:
    RegionOfInterest(int minX, int maxX, int minY, int maxY) :
        m_minX(minX), m_maxX(maxX), m_minY(minY), m_maxY(maxY) 
    {
        assert(m_maxX > m_minX && "Region must have positive width.");
        assert(m_maxY > m_minY && "Region must have positive height.");
    }

    RegionOfInterest(const RegionOfInterest& roi) :
        m_minX(roi.m_minX), m_maxX(roi.m_maxX),
        m_minY(roi.m_minY), m_maxY(roi.m_maxY) {}

    ~RegionOfInterest(){};

    RegionOfInterest& operator = (const RegionOfInterest& roi)
    {
        m_minX = roi.minX();
        m_maxX = roi.maxX();
        m_minY = roi.minY();
        m_maxY = roi.maxY();
        return *this;
    }

    bool operator == (const RegionOfInterest& roi) const
    { 
        return (m_minX == roi.m_minX && m_maxX == roi.m_maxX &&
                m_minY == roi.m_minY && m_maxY == roi.m_maxY);
    }

    bool operator != (const RegionOfInterest& roi) const
    { 
        return ! (m_minX == roi.m_minX && m_maxX == roi.m_maxX &&
                  m_minY == roi.m_minY && m_maxY == roi.m_maxY);
    }

    RegionOfInterest& operator & (const RegionOfInterest& roi)
    {
        m_minX = std::max(minX(), roi.minX());
        m_maxX = std::min(maxX(), roi.maxX());
        m_minY = std::max(minY(), roi.minY());
        m_maxY = std::min(maxY(), roi.maxY());

        return *this;
    }

    RegionOfInterest& operator | (const RegionOfInterest& roi)
    {
        m_minX = std::min(minX(), roi.minX());
        m_maxX = std::max(maxX(), roi.maxX());
        m_minY = std::min(minY(), roi.minY());
        m_maxY = std::max(maxY(), roi.maxY());

        return *this;
    }

    int centerX() const { return (m_maxX - m_minX)/2 + m_minX; }
    int centerY() const { return (m_maxY - m_minY)/2 + m_minY; }
	
    int maxX() const { return m_maxX; }
    int minX() const { return m_minX; }
    
    int maxY() const { return m_maxY; }
    int minY() const { return m_minY; }

    int height() const { return m_maxY - m_minY; }
    int width() const { return m_maxX - m_minX; }

    int area() const { return height() * width(); }
    
    double aspectRatio() const 
    {
        double boundWidth = height();
        double boundHeight = width();
        return boundWidth / boundHeight;
    }
    
    double distanceTo(const RegionOfInterest& roi) const
    {
        float x_err = roi.centerX() - centerX();
        float y_err = roi.centerY() - centerY();
        
        return sqrt(x_err*x_err + y_err* y_err);
    }    

    double distanceSignificance(const RegionOfInterest& roi) const
    {
        // this is a very rough measure of how significant
        // the distance between two regions is. bigger regions
        // lead to lower significance and bigger distance 
        // increases significance.

        double dist = distanceTo(roi);
        return dist / (area() + roi.area());
    }
    
    bool intersects(const RegionOfInterest& roi) const
    {
        if(m_minX > roi.m_maxX || m_maxX < roi.m_minX ||
           m_minY > roi.m_maxY || m_maxY < roi.m_minY)
            return false;
        else
            return true;
    }

    static RegionOfInterest fromUnion(const RegionOfInterest& first,
                                      const RegionOfInterest& second)
    {
        return RegionOfInterest(
            std::min(first.minX(), second.minX()),
            std::max(first.maxX(), second.maxX()),
            std::min(first.minY(), second.minY()),
            std::max(first.maxY(), second.maxY()));
    }

    static RegionOfInterest fromIntersection(const RegionOfInterest& first,
                                             const RegionOfInterest& second)
    {
        return RegionOfInterest(
            std::max(first.minX(), second.minX()),
            std::min(first.maxX(), second.maxX()),
            std::max(first.minY(), second.minY()),
            std::min(first.maxY(), second.maxY()));
    }

    static RegionOfInterest fromDilation(const RegionOfInterest& initialROI,
                                         int dilationPixels)
    {
        return RegionOfInterest(
            initialROI.minX() - dilationPixels,
            initialROI.maxX() + dilationPixels,
            initialROI.minY() - dilationPixels,
            initialROI.maxY() + dilationPixels);
    }

    static RegionOfInterest fromScaling(const RegionOfInterest& initialROI,
                                        double scaleFactor)
    {
        int halfWidthChange = (initialROI.width() * scaleFactor
                           - initialROI.width()) / 2;
        int halfHeightChange = (initialROI.height() * scaleFactor
                            - initialROI.height()) / 2;

        return RegionOfInterest(initialROI.minX() - halfWidthChange,
                                initialROI.maxX() + halfWidthChange,
                                initialROI.minY() - halfHeightChange,
                                initialROI.maxY() + halfHeightChange);
    }

private:
    int m_minX;
    int m_maxX;
    int m_minY;
    int m_maxY;
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_REGION_OF_INTEREST

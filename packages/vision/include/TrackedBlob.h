/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/TrackedBlob.h
 */


#ifndef RAM_TRACKEDBLOB_H_05_15_2009
#define RAM_TRACKEDBLOB_H_05_15_2009

// Project Includes
#include "vision/include/BlobDetector.h"
#include "math/include/Math.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
        
class RAM_EXPORT TrackedBlob : public BlobDetector::Blob
{
public:
    TrackedBlob();
    TrackedBlob(BlobDetector::Blob blob, double x, double y,
                math::Degree rotation, int id);
    TrackedBlob(BlobDetector::Blob blob, double x, double y, int id);

    TrackedBlob(BlobDetector::Blob blob, Image* sourceImage,
                math::Degree rotation, int id);
    TrackedBlob(BlobDetector::Blob blob, Image* sourceImage, int id);
    
    /** Gets the center in normalized cordinates -1 -> 1 */
    double getX(){ return m_normX; }
    
    /** Gets the center in normalized cordinates -1 -> 1 */
    double getY() { return m_normY; }
    
    /** Angle of vertical (not always applicable) */
    math::Degree getAngle(){ return m_angle;}
    
    void setAngle(math::Degree ang) { m_angle = ang; } //Does this work?
    
    int getId() const { return m_id; }
    
    /** Computes the distance between the blob centers */
    double distanceTo(TrackedBlob& otherTrackedBlob);
    
    /** Distance from normalized cordinates (0,0) center, -1 -> 1 */
    double distanceTo(double x, double y);
    
    bool operator==(const TrackedBlob& other) {
        return (m_id == other.getId()); }
    
    void _setId(int id) { m_id = id; }
    
    /** Draws the normal states but includes the ID */
    void drawStats(Image* image);
    
private:
    double m_normX;
    double m_normY;
    
    math::Degree m_angle;
    int m_id;
};
    
} // namespace vision
} // namespace ram

#endif // RAM_TRACKEDBLOB_H_05_15_2009


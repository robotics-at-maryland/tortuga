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

/** A BlobDetector::Blob with an ID so it can be tracked between frames
 *
 *  In addition to all the normal Blob information it has the blob center in
 *  image coordinates, and a unique ID. It also has helper methods that
 *  can compare two lists of TrackedBlobs and update there ID's so that any
 *  blob which has not moved or significantly will have the same ID.
 *  TrackedBlobs are created from Blobs created by the standard BlobDetector.
 */  
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
    double getX() const { return m_normX; }
    
    /** Gets the center in normalized cordinates -1 -> 1 */
    double getY() const { return m_normY; }
    
    /** Angle of vertical (not always applicable) */
    math::Degree getAngle() const { return m_angle;}
    
    void setAngle(math::Degree ang) { m_angle = ang; } //Does this work?
    
    int getId() const { return m_id; }
    
    /** Computes the distance between the blob centers */
    double distanceTo(const TrackedBlob& otherTrackedBlob) const;
    
    /** Distance from normalized cordinates (0,0) center, -1 -> 1 */
    double distanceTo(double x, double y) const;
    
    bool operator==(const TrackedBlob& other) const {
        return (m_id == other.getId()); }

    bool operator<(const TrackedBlob& other) const {
        return (m_id < other.getId()); }

    bool operator>(const TrackedBlob& other) const {
        return (m_id > other.getId()); }
    
    void _setId(int id) { m_id = id; }
    
    /** Draws the normal states but includes the ID */
    void drawStats(Image* image);

    /** Matches blobs in the newList to those in the oldList
     *
     *  The oldList will have all blobs that are matched to new blobs removed.
     *  All blobs in the newList with matches will have there IDs updated to
     *  reflect that. Blobs must be within the distanceThreshold to be
     *  considered the same blob.
     *
     *  @param oldList
     *      Starts with all the Blobs from last frame, ends up with all the
     *      blobs not in this frame.
     *  @param newList
     *      All the newly found blobs in this frame.
     *  @param distanceThreshold
     *      Blob centers must be within this distance in image coordinates to
     *      be considered the same blob.
     */
    template <class T>
    static void updateIds(T* oldList, T* newList, double distanceThreshold);

    /** Matches blobs in the newList with the oldList but with history
     *
     *  This performs similar to the normal updateIds, but it doesn't move
     *  Blobs to the oldList until they have been gone for lostFrameCount
     *  number of frames.
     *
     *  @param oldList
     *      Passed in with all the Blobs from last frame, ends up with all the
     *      blobs that have not been seen for lostFrameCount frames.
     *  @param newList
     *      All the newly found blobs in this frame.
     *  @param lostBlobMap
     *      Maps Blob to how many frames it has *left* before being put in the
     *      oldList.
     *  @param distanceThreshold
     *      Blob centers must be within this distance in image coordinates to
     *      be considered the same blob.
     *  @param lostFrameCount
     *      How many updates a blob has to not be seen in order for it to moved
     *      the oldList. 0 = same behavior as standard updateIds.
     */
    template <class T, class K>
    static void updateIds(T* oldList, T* newList, K* lostBlobMap,
                          double distanceThreshold,
                          int lostFrameCount);

    
private:
    double m_normX;
    double m_normY;
    
    math::Degree m_angle;
    int m_id;
};
    
template <class T>
void TrackedBlob::updateIds(T* oldList, T* newList, double distanceThreshold)
{
    typename T::iterator newListIter = newList->begin();
    typename T::iterator newListEnd = newList->end();
    for (;newListIter != newListEnd; newListIter++)
    {
        // Go through the list of current bins and find the closest bin
        double currentMin = 10000;
        typename T::iterator currentBlob = oldList->begin();
        typename T::iterator oldListIter = oldList->begin();
        typename T::iterator oldListEnd = oldList->end();
        for (;oldListIter != oldListEnd; oldListIter++)
        {
            double distance = oldListIter->distanceTo(*newListIter);
            if (distance < currentMin)
            {
                currentMin = distance;
                currentBlob = oldListIter;
            }
        }
        
        // If its close enough, we transfer the ID
        if (currentMin < distanceThreshold)
        {
            // Transfer Id
            newListIter->_setId(currentBlob->getId());
            // Remove from list to search against
            oldList->erase(currentBlob);
        }
    }
}

template <class T, class K>
void TrackedBlob::updateIds(T* oldList, T* newList, K* lostBlobMap,
                            double distanceThreshold,
                            int lostFrameCount)
{
    typename T::iterator newListIter = newList->begin();
    typename T::iterator newListEnd = newList->end();
    for (;newListIter != newListEnd; newListIter++)
    {
        // Go through the list of current blobs and find the closest blob
        double currentMin = 10000;
        typename T::iterator currentBlob = oldList->begin();
        typename T::iterator oldListIter = oldList->begin();
        typename T::iterator oldListEnd = oldList->end();
        for (;oldListIter != oldListEnd; oldListIter++)
        {
            double distance = oldListIter->distanceTo(*newListIter);
            if (distance < currentMin)
            {
                currentMin = distance;
                currentBlob = oldListIter;
            }
        }

        // Go through the list of previous blobs and find the closest blob
        double mapMin = 10000;
        typename K::iterator mapBlob = lostBlobMap->begin();
        typename K::iterator oldMapIter = lostBlobMap->begin();
        typename K::iterator oldMapEnd = lostBlobMap->end();
        for (;oldMapIter != oldMapEnd; oldMapIter++)
        {
            double distance = oldMapIter->first.distanceTo(*newListIter);
            if (distance < mapMin)
            {
                mapMin = distance;
                mapBlob = oldMapIter;
            }
        }

        // If its close enough, we transfer the ID
        if ((currentMin < distanceThreshold) && (currentMin <= mapMin))
        {
            // Transfer Id
            newListIter->_setId(currentBlob->getId());
            // Remove from list to search against
            oldList->erase(currentBlob);
        }
        else if ((mapMin < distanceThreshold) && (mapMin < currentMin))
        {
            // Transfer Id
            newListIter->_setId(mapBlob->first.getId());
            // Remove from map of previous blobs we are still trying to map
            lostBlobMap->erase(mapBlob);
        }
    } // foreach blob in newList

    if (lostFrameCount > 0)
    {
        // Move all the dropped bins the map
        typename T::iterator oldListIter = oldList->begin();
        typename T::iterator oldListEnd = oldList->end();
        for (;oldListIter != oldListEnd; oldListIter++)
        {
            // The +1 is because its going to be decremented in the next loop
            lostBlobMap->insert(
                typename K::value_type(*oldListIter, lostFrameCount));
        }
        oldList->clear();
        
        // Now go through and decrement the count for all the blobs and
        // add the ones to remove to the list of oldBlobs
        typename K::iterator oldMapIter = lostBlobMap->begin();
        typename K::iterator oldMapEnd = lostBlobMap->end();
        for (;oldMapIter != oldMapEnd; oldMapIter++)
        {
            if (oldMapIter->second == 0)
            {
                // We have lost this blob, report it
                oldList->push_back(oldMapIter->first);
            }
            else
            {
                // Decrement how many frames its been lost
                oldMapIter->second -= 1;
            }
        }

        // Removed the dropped blobs from the map
        oldListIter = oldList->begin();
        oldListEnd = oldList->end();
        for (;oldListIter != oldListEnd; oldListIter++)
        {
            lostBlobMap->erase(*oldListIter);
        }
    } // frameCount > 0
}

    
} // namespace vision
} // namespace ram

#endif // RAM_TRACKEDBLOB_H_05_15_2009


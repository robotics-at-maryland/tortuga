/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestOrangePipeDetector.cxx
 */

// STD Includes
#include <map>
#include <vector>
#include <iostream>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/assign.hpp>

// Project Includes
#include "vision/include/TrackedBlob.h"

//#include "vision/test/include/Utility.h"

namespace ba = boost::assign;
using namespace ram;

typedef std::vector<vision::TrackedBlob> BlobList;
typedef std::map<vision::TrackedBlob, int> LostBlobMap;

std::ostream& operator<<(std::ostream& stream, vision::TrackedBlob b)
{
    stream << b.getId() << "(" << b.getX() << "," << b.getY() << ")";

    return stream;
}

SUITE(TrackedBlob) {

TEST(updateIds)
{
    vision::BlobDetector::Blob dummyBlob;

    // Input
    BlobList oldList = ba::list_of(vision::TrackedBlob(dummyBlob, 0, 0, 1))
        (vision::TrackedBlob(dummyBlob, 1, 1, 2));

    BlobList newList = ba::list_of(vision::TrackedBlob(dummyBlob, 0.1, 0.1, 3))
        (vision::TrackedBlob(dummyBlob, -1, -1, 4));

    vision::TrackedBlob::updateIds(&oldList, &newList, 0.25);

    // Expected output
    BlobList expectedOld = ba::list_of(vision::TrackedBlob(dummyBlob, 1, 1, 2));

    BlobList expectedNew = ba::list_of
        (vision::TrackedBlob(dummyBlob, 0.1, 0.1, 1))
        (vision::TrackedBlob(dummyBlob, -1, -1, 4));

    CHECK_EQUAL(expectedOld.size(), oldList.size());
    for (unsigned int i = 0; i << expectedOld.size(); ++i)
        CHECK_EQUAL(expectedOld[i], oldList[i]);

    CHECK_EQUAL(expectedNew.size(), newList.size());
    for (unsigned int i = 0; i << expectedNew.size(); ++i)
        CHECK_EQUAL(expectedNew[i], newList[i]);
}

TEST(updateIdsWithMap)
{
    vision::BlobDetector::Blob dummyBlob;

    // Input
    LostBlobMap lostBlobs = ba::map_list_of
        (vision::TrackedBlob(dummyBlob, 5, 5, 1), 0) // To be lost
        (vision::TrackedBlob(dummyBlob, 0.5, 0.5, 2), 0) // To be linked up
        (vision::TrackedBlob(dummyBlob, 2, 2, 22), 1); // To be decremeted
    
    BlobList oldList = ba::list_of(vision::TrackedBlob(dummyBlob, 0, 0, 3))
        (vision::TrackedBlob(dummyBlob, 1, 1, 4)); // To be lost

    BlobList newList = ba::list_of(vision::TrackedBlob(dummyBlob, 0.1, 0.1, 5))
        (vision::TrackedBlob(dummyBlob, -1, -1, 6))
        (vision::TrackedBlob(dummyBlob, 0.6, 0.6, 7));

    vision::TrackedBlob::updateIds(&oldList, &newList, &lostBlobs, 0.25, 1);

    // Expected output
    BlobList expectedOld = ba::list_of(vision::TrackedBlob(dummyBlob, 5, 5, 1));

    BlobList expectedNew = ba::list_of
        (vision::TrackedBlob(dummyBlob, 0.1, 0.1, 3))
        (vision::TrackedBlob(dummyBlob, -1, -1, 6))
        (vision::TrackedBlob(dummyBlob, 0.6, 0.6, 2));

    CHECK_EQUAL(expectedOld.size(), oldList.size());
    for (unsigned int i = 0; i << expectedOld.size(); ++i)
        CHECK_EQUAL(expectedOld[i], oldList[i]);

    CHECK_EQUAL(expectedNew.size(), newList.size());
    for (unsigned int i = 0; i << expectedNew.size(); ++i)
        CHECK_EQUAL(expectedNew[i], newList[i]);

    CHECK_EQUAL(2u, lostBlobs.size());
    vision::TrackedBlob oldBlob(dummyBlob, 2, 2, 22);
    vision::TrackedBlob newOldBlob(dummyBlob, 1, 1, 4);
    CHECK(lostBlobs.end() != lostBlobs.find(oldBlob));
    CHECK(lostBlobs.end() != lostBlobs.find(newOldBlob));
    CHECK_EQUAL(0, lostBlobs[oldBlob]);
    CHECK_EQUAL(0, lostBlobs[newOldBlob]);
}

} // SUITE(TrackedBlob)

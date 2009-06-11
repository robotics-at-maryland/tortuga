/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/RawFileRecorder.h
 */

#ifndef RAM_RAWFILERECORDER_H_06_11_2009
#define RAM_RAWFILERECORDER_H_06_11_2009

// Project Includes
#include "vision/include/Recorder.h"

// Boost Includes
#include <boost/cstdint.hpp>

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

class RAM_EXPORT RawFileRecorder : public Recorder
{
 
  public:
    static const boost::uint32_t MAGIC_NUMBER;
    
    struct Header
    {
        boost::uint32_t magicNumber;
        boost::uint32_t width;
        boost::uint32_t height;
        /** Size of the packet header as well as the data */
        boost::uint32_t packetSize;
        boost::uint32_t format;
        double framerate;
    };

    struct Packet
    {
        boost::uint32_t framenum;
//        double timestamp;
        boost::uint32_t size;
    };

    
    RawFileRecorder(Camera* camera, Recorder::RecordingPolicy policy,
                 std::string rawfilename, int policyArg = 0);

    virtual ~RawFileRecorder();

  protected:
    /** Called whenever there is a frame to record, records to disk */
    virtual void recordFrame(Image* image);
    
  private:
    /** File we are writing our data to */
    FILE* m_file;

    /** Current frame we are writing to disk */
    boost::uint32_t m_framenum;
};
    
} // namespace vision
} // namespace ram	

#endif // RAM_RAWFILERECORDER_H_06_11_2009

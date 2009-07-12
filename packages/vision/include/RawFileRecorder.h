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
    static const boost::uint32_t RMV_VERSION;
    
    struct Header
    {
        /** Here to make sure we are reading from the right kind of file*/
        boost::uint32_t magicNumber;
        /** Version of the file format */
        boost::uint32_t versionNumber;
        /** Width in pixesl of the images in the file */
        boost::uint32_t width;
        /** Height in pixesl of the images in the file */
        boost::uint32_t height;
        /** Size of the packet header as well as the data */
        boost::uint32_t packetSize;
        /** Image::PixelFormat of the images */
        boost::uint32_t format;
        /** Number of frames per second in the stream */
        double framerate;
        
        // Here to allow room for expansion
        double unusedDouble;
        boost::uint32_t unusedInt1;
        boost::uint32_t unusedInt2;
    };

    struct Packet
    {
        /** Helps find seek errors when looking through packets */
        boost::uint32_t magicNumber;
        /** Size of the packet header as well as the data of previous packet */
        boost::uint32_t lastPacketSize;
        /** The frame number in our total sequence of frames */
        boost::uint32_t framenum;
        /** The size of the frame data coming next */
        boost::uint32_t dataSize;

        // Here to allow room for expansion
        double unusedDouble;
        boost::uint32_t unusedInt1;
        boost::uint32_t unusedInt2;
    };

    
    RawFileRecorder(Camera* camera, Recorder::RecordingPolicy policy,
                    std::string rawfilename, int policyArg = 0,
                    int recordWidth = 640, int recordHeight = 480);

    virtual ~RawFileRecorder();

  protected:
    /** Called whenever there is a frame to record, records to disk */
    virtual void recordFrame(Image* image);
    
  private:
    /** File we are writing our data to */
    FILE* m_file;

    /** Current frame we are writing to disk */
    boost::uint32_t m_framenum;

    /** The total size of the last packet written (header and data) */
    boost::uint32_t m_lastPacketSizeWritten;
};
    
} // namespace vision
} // namespace ram	

#endif // RAM_RAWFILERECORDER_H_06_11_2009

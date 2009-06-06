/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/FileRecorder.h
 */

#ifndef RAM_FILERECORDER_H_02_24_2008
#define RAM_FILERECORDER_H_02_24_2008

// Project Includes
#include "vision/include/Recorder.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

class RAM_EXPORT FileRecorder : public Recorder
{
  public:
    FileRecorder(Camera* camera, Recorder::RecordingPolicy policy,
                 std::string filename, int policyArg = 0);

    virtual ~FileRecorder();

  protected:
    /** Called whenever there is a frame to record, records to disk */
    virtual void recordFrame(Image* image);
    
  private:
    /** OpenCV handle to video writer structure */
    CvVideoWriter* m_writer;
};
    
} // namespace vision
} // namespace ram	

#endif // RAM_FILERECORDER_H_02_24_2008

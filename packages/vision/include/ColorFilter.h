/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/ColorFilter.h
 */

#ifndef RAM_VISION_COLORFILTER_H_04_28_2009
#define RAM_VISION_COLORFILTER_H_04_28_2009

// Project Includes
#include "core/include/Forward.h"
#include "vision/include/Common.h"

// Must be incldued last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

class RAM_EXPORT ColorFilter
{
public:
    ColorFilter(unsigned char channel1Low, unsigned char channel1High,
                unsigned char channel2Low, unsigned char channel2High,
                unsigned char channel3Low, unsigned char channel3High);

    virtual ~ColorFilter();

    /** Run the Filter on the input image, debug results to output Image
     *
     *  @param input   The image to run the detector on
     *  @param output  Debug image will be copied to this image
     */
    virtual void filterImage(Image* input);

private:
    unsigned char m_channel1Range[255];
    unsigned char m_channel2Range[255];
    unsigned char m_channel3Range[255];
};
    
} // namespace vision
} // namespace ram

#endif // RAM_VISION_COLORFILTER_H_04_28_2009

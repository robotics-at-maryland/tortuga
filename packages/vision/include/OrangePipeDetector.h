/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/OrangePipeDetector.h
 */

#ifndef RAM_ORANGE_PIPE_DETECTOR_H_06_23_2007
#define RAM_ORANGE_PIPE_DETECTOR_H_06_23_2007

// STD Includes
#include <set>
#include <string>

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/PipeDetector.h"
#include "core/include/ConfigNode.h"
#include "math/include/Math.h"
#include "vision/include/TableColorFilter.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
        
class RAM_EXPORT OrangePipeDetector  : public PipeDetector
{
  public:
    OrangePipeDetector(core::ConfigNode config,
                       core::EventHubPtr eventHub = core::EventHubPtr());
    ~OrangePipeDetector();
    
    void processImage(Image* input, Image* output= 0);
    
    bool found();

    /** Get normalized X cordinate of the center of the orange line */
    double getX();

    /** Get normalized Y cordinate of the center of the orange line */
    double getY();

    math::Degree getAngle();

    /** Set whether or not to use the LUV filter */
    void setUseLUVFilter(bool value);
    
    // Setter and Getter for lookup table color filter
    bool getLookupTable();
    void setLookupTable(bool lookupTable);
  
  private:
    void init(core::ConfigNode config);

    /** Use Dan's custom mask_orange function */
    void filterForOrangeOld(Image* image);

    /** Use LUV color mask function  */
    void filterForOrangeNew(Image* image);
    
    /** Use the color filter to filter for orange */
    //    void filterForOrange();

    /** Angle of the pipe */
    math::Radian m_angle;
    
    /** X cordinate of pipe */
    double m_lineX;
    
    /** Y cordinate of pipe */
    double m_lineY;

    double m_rOverGMin;
    double m_rOverGMax;
    double m_bOverRMax;
    bool m_found;

    /** Filters for orange */
    ColorFilter* m_filter;

    /** table color filter */
    TableColorFilter *m_tableColorFilter;

    /** Whether or not to use the newer LUV color filter */
    bool m_useLUVFilter;
    
    /** Maximum distance for the pipe to be considred "centered" */
    double m_centeredLimit;

    /** Whether or not we are centered */
    bool m_centered;

    /** The minimum brightness a pixel has to be to be considered orange */
    int m_minBrightness;

    /** Number of times to erode the masked image before the hough */
    int m_erodeIterations;

    /** The set of IDs of the pipes that were present in the last frame */
    std::set<int> m_lastPipeIds;

    bool m_colorFilterLookupTable;
    
    std::string m_lookupTablePath;
};
    
} // namespace vision
} // namespace ram

#endif // RAM_ORANGE_PIPE_DETECTOR_H_06_23_2007


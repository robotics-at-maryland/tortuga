/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/BarbedWireDetector.h
 */

#ifndef RAM_BARBED_WIRE_DETECTOR_H_05_16_2009
#define RAM_BARBED_WIRE_DETECTOR_H_05_16_2009

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/PipeDetector.h"

#include "core/include/ConfigNode.h"

// Must be incldued last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
    
class RAM_EXPORT BarbedWireDetector : public PipeDetector
{
  public:
    BarbedWireDetector(core::ConfigNode config,
                       core::EventHubPtr eventHub = core::EventHubPtr());
    ~BarbedWireDetector();

    void processImage(Image* input, Image* output= 0);

    bool found();

    /** The percent of the top of the image blacked out */
    void setTopRemovePercentage(double percent);

    /** The percent of the bottom of the image blacked out */
    void setBottomRemovePercentage(double percent);
        
  private:
    void init(core::ConfigNode config);

    /** Filters the image so that all green is white
     *
     *  Note the applies the top and bottom remove steps as well as the desired
     *  erosion and dilation steps.  It produces an image ready to be processed
     *  by the other portions of the algorithm.
     */
    void filterForGreen(Image* input);
    
    /** Processes the list of all found pipes and finds all valid pipes
     *
     *  This current only checks for based on the angle of the pipes.
     */
    bool processPipes(const PipeDetector::PipeList& pipes,
                      PipeDetector::PipeList& outPipes);


    /** Sorts pipe by there X coordinate, largest (highest in image) first */
    static bool pipeXSorter(PipeDetector::Pipe pipe1, PipeDetector::Pipe pipe2);
    
    /** Filters for the green in the water */
    ColorFilter* m_filter;

    /** Working scratch image */
    Image* m_image;

    /** Wether or not the barbed is found */
    bool m_found;

    /** Minimum angle for the barbed wire pipe */
    double m_minAngle;
    
    /** How many times to erode the green filtered elements */
    int m_erodeIterations;

    /** How many times to dilate the green filtered elements */
    int m_dilateIterations;
    
    /** Percentage of the image to remove from the top */
    double m_topRemovePercentage;

    /** Percentage of the image to remove from the bottom */
    double m_bottomRemovePercentage;
};
	
} // namespace vision
} // namespace ram

#endif // RAM_BARBED_WIRE_DETECTOR_H_05_16_2009

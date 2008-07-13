/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/BinDetector.h
 */

#ifndef RAM_BIN_DETECTOR_H_06_23_2007
#define RAM_BIN_DETECTOR_H_06_23_2007

// Project Includes
#include "core/include/ConfigNode.h"
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "vision/include/BlobDetector.h"
#include "vision/include/SuitDetector.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
    
class RAM_EXPORT BinDetector : public Detector
{
  public:
    BinDetector(core::ConfigNode config,
                core::EventHubPtr eventHub = core::EventHubPtr());
    BinDetector(Camera*);
    ~BinDetector();

    void processImage(Image* input, Image* output= 0);
    void update();
    void show(char* window);
    bool found();
    IplImage* getAnalyzedImage();

    /** X cord of the bin closest to the center of the screen */
    float getX();
    /** Y cord of the bin closest to the center of the screen */
    float getY();
    
  private:
    void init(core::ConfigNode config);

    /** Processes the bin and fires off found event
     *
     *  @param bin
     *      The blob which bounds the black box of the bin
     *  @oaram detectoSuit
     *      True if we are to find the suit for suit detection
     *  @param output
     *      Our debug output image
     */
    void processBin(BlobDetector::Blob bin, bool detectSuit, Image* ouput = 0);

    /** Called by process bin, if suit detection is request is true
     *
     *  @param bin
     *      The blob which bounds the black box of the bin
     *  @param input
     *      The image to extract the suit from
     *  @param outptu
     *      Our debug output image
     */
    Suit::SuitType determineSuit(BlobDetector::Blob bin, Image* input,
                                 Image* output = 0);
    
    bool m_found;
/*    bool foundHeart;
    bool foundSpade;
    bool foundDiamond;
    bool foundClub;
    bool foundEmpty;*/
    /** Determines if we should try and determine the suit of the bin */
    bool m_runSuitDetector;
    int numBinsFound;
    IplImage* binFrame;
    IplImage* rotated;
    IplImage* bufferFrame;
    IplImage* whiteMaskedFrame;
    IplImage* blackMaskedFrame;
    float binX, binY;
    Image* frame;
    Camera* cam;
    SuitDetector suitDetector;
    BlobDetector blobDetector;
    /** Maximum distance for the bin to be considred "centered" */
    double m_centeredLimit;
    
    /** Whether or not we are centered */
    bool m_centered;
    
    Suit suitCenteredOver;
    
};

} // namespace vision
} // namespace ram

#endif // RAM_BIN_DETECTOR_H_06_23_2007

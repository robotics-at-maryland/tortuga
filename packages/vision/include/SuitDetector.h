/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/SuitDetector.h
 */

#ifndef RAM_SUIT_DETECTOR_H_04_29_2008
#define RAM_SUIT_DETECTOR_H_04_29_2008

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "vision/include/BlobDetector.h"
#include "vision/include/Symbol.h"

#include "core/include/ConfigNode.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
    
class RAM_EXPORT SuitDetector : public Detector
{
  public:
    Symbol::SymbolType suit;
    SuitDetector(core::ConfigNode config,
                 core::EventHubPtr eventHub = core::EventHubPtr());
    SuitDetector(Camera*);
    ~SuitDetector();
    void processImage(Image* input, Image* output= 0);
    void update();
    IplImage* getAnalyzedImage();
    Symbol::SymbolType getSymbol();
    //Fills numBackups if numBackups!=null
    int edgeRun(int startx, int starty, int endx, int endy, IplImage* img, int dir, int* numBackups = 0);
    void doEdgeRunning(IplImage*);
    bool makeSuitHistogram(IplImage*);
	bool cropImage(IplImage*);
    bool cyclicCompare(int[], int[], int[], int);
  private:
    Camera* cam;
    IplImage* ratioImage;
    IplImage* analyzedImage;
	IplImage* tempHoughImage;
    void init(core::ConfigNode config);
    BlobDetector blobDetector;
    int findPointsOnEdges2(IplImage* img, int xPositions[], int yPositions[]);
    int findPointsOnEdges(IplImage* img, int xPositions[], int yPositions[]);

    int histoArr[128]; //twice scaledRedSuit's height.
    static const int HISTOARRSIZE = 128;
    static int HEARTMIN[]; 
    static int HEARTMAX[]; 
    static int HEARTSIZE;

    static int SPADEMIN[]; 
    static int SPADEMAX[]; 
    static int SPADESIZE; 

    static int CLUBMIN[];
    static int CLUBMAX[];
    static int CLUBSIZE;

    static int DIAMONDMIN[];
    static int DIAMONDMAX[];
    static int DIAMONDSIZE;

    static int SPLITMIN[];
    static int SPLITMAX[];
    static int SPLITSIZE;
};
	
} // namespace vision
} // namespace ram

#endif // RAM_SUIT_DETECTOR_H_04_29_2008

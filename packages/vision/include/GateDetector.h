/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/GateDetector.h
 */

#ifndef RAM_GATE_DETECTOR_H_06_23_2007
#define RAM_GATE_DETECTOR_H_06_23_2007

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "core/include/ConfigNode.h"
#include "vision/include/GateDetectorKate.h"
#include "vision/include/BuoyDetectorKate.h"
// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
    

struct contourblob
{
	double angle;
	int contournumber;
	int area;
	int width;
	int height;
	int centerx;
	int centery;
	int gatepieces;
};

class RAM_EXPORT GateDetector : public Detector
{
  public:
    bool found;
    GateDetector(core::ConfigNode config,
                 core::EventHubPtr eventHub = core::EventHubPtr());
    GateDetector(Camera*);
    ~GateDetector();
    
    void update();
    void processImage(Image* input, Image* output= 0);
    void publishFoundEvent(foundLines::parallelLinesPairs finalPairs);
    Mat processImageColor(Image* input);
    void publishFoundEventBuoy(foundLines::parallelLinesPairs finalPairs, Color::ColorType color); //hack to trick event system
    void publishLostEventBuoy(Color::ColorType color);
    double getX();
    double getY();
    void show(char* window);
    IplImage* getAnalyzedImage();
    blobfinder blob;
    void FindShape(Mat erosion_dst, Mat img, Mat ref_canny);
    Mat GetReference(int refnumber);
    Mat GetOutline(Mat img);
    Mat m_ref_canny;
    Mat m_ref_img;
    Mat m_sal_output;

 int returnRedmin(void);
int returnRedmax(void);
int getmaxdiff(); //gets the maximum allowed difference for kate function
int m_maxdiff;
int m_refcontour;

   
    
  private:

bool m_found;
    void init(core::ConfigNode config);
      ColorFilter* m_filter;
/**Filter for Red value for VisionToolV2*/

	//Mat img_whitebalance;    

	int m_redminH;
	int m_redmaxH;
	int m_greenminH;
	int m_greenmaxH;
	int m_minS;
	int m_maxS;
	int m_minV;
	int m_maxV;
	bool m_checkRed;

    int gateX;
    int gateY;
    double gateXNorm;
    double gateYNorm;
    IplImage* gateFrame;
    IplImage* gateFrameRatios;
    
    Image* frame;
    Camera* cam;


/*Kate edit - same whitebalance image and final output*/
	cv::Mat img_whitebalance;
	cv::Mat img_gate;
	foundLines gate;
	int m_erosion_size;
	int m_maxanglediff;
	void FindContours(cv::Mat img_src);
	double m_minAspectRatio;
	double m_maxAspectRatio;

	int m_refimage;
	int m_minrefarea;
	int m_maxrefarea;
	double m_aspectratio;
	double m_bestMatch;
	double m_arearatio;

	double m_arclengthratio;
	int m_minArea;
	
	void publishFoundEventContour(contourblob contour, Color::ColorType color);
	contourblob finalGate; //final gate information using contours
	int m_reflectiondiff;
	int m_cannylow;
	int m_cannyhigh;
	int m_minY;
	int m_morphsize;
	int m_morphtype;
	int m_erodesize;
	int m_dilatesize;
	int m_cannyoradaptive;
	int m_adwindow;
	double m_threshvalue;
	cv::Mat m_saliency;
	void SaliencyFilter(cv::Mat img);
};
    
} // namespace vision
} // namespace ram

#endif // RAM_GATE_DETECTOR_H_06_23_2007

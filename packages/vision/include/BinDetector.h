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

// STD Includes
#include <list>
#include <map>
#include <string>

// Project Includes
#include "core/include/ConfigNode.h"
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "vision/include/BlobDetector.h"
#include "vision/include/TrackedBlob.h"
#include "vision/include/Symbol.h"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/features2d/features2d.hpp"

#include "vision/include/GateDetectorKate.h"
//#include "vision/include/BuoyDetectorKate.h"

// Must be included last
#include "vision/include/Export.h"


namespace ram {
namespace vision {
    
class RAM_EXPORT BinDetector : public Detector
{
	struct contourvertices
	{
		cv::Point2f vertices[4];
	};

	struct bincontours
	{
		int area;
		int contournumber;
		double aspectratio_diff;
		double maxX;
		double maxY;
		double minY;
		double minX;
		bool found;
		cv::Point2f vertices[4];
		int centerx;
		int centery;
		bool identified;
		int type;
		double angle;
		double width;
		double height;
		
	};
	struct binData
	{

		bool Box_found;
		int Box_x;
		int Box_y;
		double Box_angle;	
		int Box_height;
		int Box_width;
		bool Box_identified; //is it identified
		int Box_numberofframes; //number of frames consistently id'd
		int Box_type; //identification number
		
	};
	struct finalBins
	{
		bool MainBox_found;
		int MainBox_x;
		int MainBox_y;
		double MainBox_angle;
		int MainBox_height;
		int MainBox_width;
		int MainBox_type; //identification number
		binData Box[4];
	};

	
int m_adwindow;
  public:
    class Bin : public TrackedBlob
    {
    public:
        Bin();
        Bin(BlobDetector::Blob blob, Image* source,
            math::Degree rotation, int id,  Symbol::SymbolType symbol);

        Symbol::SymbolType getSymbol() { return m_symbol; }

        /** Draws the bounds of the bin in green, and its ID */
        void draw(Image* image, Image* red = 0);

    private:
        Symbol::SymbolType m_symbol;
    };

    typedef std::list<Bin> BinList;
    typedef std::map<Bin, int> BinLostMap;
    typedef BinList::iterator BinListIter;
    
    BinDetector(core::ConfigNode config,
                core::EventHubPtr eventHub = core::EventHubPtr());


    ~BinDetector();

    void processImage(Image* input, Image* output= 0);
    
    bool found();

    /** X cord of the bin closest to the center of the screen */
    float getX();
    
    /** Y cord of the bin closest to the center of the screen */
    float getY();

    /** angle of the bin closest to the center of the screen*/
    math::Degree getAngle();

    /** Gets the symbol of the bin cloest to the center of the screen */
    Symbol::SymbolType getSymbol();
    
    /** Gets our current set of bins, sorted close to farther from center */
    BinDetector::BinList getBins();
    
    void setSymbolDetectionOn(bool);

    /** Turns on and off the logging of properly extracted symbol images to disk
     *
     *  This places all the symbol images in <log_dir>/symbols/<symbol_type>
     *  directory.  Where <log_dir> is the runs log directory, and <symbol_type>
     *  is heart, spade, club, diamond, or unknown, or other newer symbols.
     *
     *  This only works when you are display debug images as well.
     *
     *  @param value
     *      True if you wish to log, false if not.
     */
    void setSymbolImageLogging(bool value);

  private:
    /** Initializes all the values that are config based */
    void init(core::ConfigNode config);

    /** Allocates all the scratch images */
    void allocateImages(int width, int height);

    /** Release all the scratch images */
    void deleteImages();
    
    /** Filters the image all white is white, and everything else is black */
    void filterForWhite(Image* input, Image* output);

    /** Filters the image all white is white, and everything else is black */
    void filterForBlack(Image* input, Image* output);

    /** Filters the image all red is white, and everything else is black */
    void filterForRed(Image* input, Image* output);

    /** Places debug information from the colour filters on the output */
    void filterDebugOutput(Image* output);

    /** Finds all black blobs that are inside white blobs (ie. bins) */
    void findBinBlobs(const BlobDetector::BlobList& whiteBlobs,
                      const BlobDetector::BlobList& blackBlobs,
                      BlobDetector::BlobList& binBlobs,
                      Image* output);
    
    /** Determines the angle of the bin array (draws debug output if needed)
     *
     *  @param bins
     *      The bins that make up the array we find the angle of
     *  @param angle
     *      The output parameter, set to the angle of the array
     *  @param output
     *      The image we write our debug information to
     *  @return
     *      True if its a valid angle that should be reported
     */
    bool findArrayAngle(const BinList& bins, math::Degree& angle,
                        Image* output);
    
    /** Processes the bin and fires off found event
     *
     *  It adds each process bin to the list of current bins.
     *
     *  @param bin
     *      The blob which bounds the black box of the bin
     *  @oaram detectorSymbol
     *      True if we are to find the symbol for symbol detection
     *  @param binNum
     *      The number of our bin in the array (0 - 3)
     *  @param output
     *      Our debug output image
     *
     *  @return
     *      The bin structure containing infomation on the bin
     */
    BinDetector::Bin processBin(BlobDetector::Blob bin, bool detectSymbol,
                                int binNum, Image* ouput);

    /** Finds the percentage of the bin that is red pixel */
    static double getRedFillPercentage(BlobDetector::Blob bin, Image* redImage);

    /** Called by process bin, must be called regardless of whether we plan
     *  to detect symbols, as this function sets the angle of the bin.
     *
     *  @return
     *      True if we succeeded in finding the angle
     */
    bool calculateAngleOfBin(BlobDetector::Blob bin, Image* input,
                             math::Degree& foundAngle, Image* output = 0);

    /** Crops the incoming image down so its a square surounding the symbol
     *
     *  @param redBinImage
     *      The image to crop
     *  @param storageBuffer
     *      The buffer to use to store the result image in
     *
     *  @return
     *       0 on failure, a valid pointer if success
     */
    Image* cropBinImage(Image* redBinImage, unsigned char* storageBuffer);
    
    /** Called by process bin, if symbol detection is request is true
     *
     *  @param bin
     *      The blob which bounds the black box of the bin
     *  @param input
     *      The image to extract the symbol from
     *  @param scratchBuffer
     *      A buffer to use if a scratch image is needed
     *  @param output
     *      Our debug output image
     */
    Symbol::SymbolType determineSymbol(Image* input,
                                       unsigned char* scratchBuffer,
                                       Image* output = 0);

    /** Logs the image of the symbol to file based on the symbol type */
    void logSymbolImage(Image* image, Symbol::SymbolType symbol);
    
    /** determines the debug output */
    int m_debug;

    /** Object that detects the current on screen blobs */
    BlobDetector m_blobDetector;

    /** Object that determines the symbol in the bin */
    SymbolDetectorPtr m_symbolDetector;
    
    /** Our current set of bins */
    BinList m_bins;

    /** The bins we currently can't see but might in the future */
    BinLostMap m_lostBins;

    /** Whether or not we found any bins last frame */
    bool m_found;

    /** Whether or not we were centered last frame */
    bool m_centered;
    
    /** Maximum distance for the bin to be considred "centered" */
    double m_centeredLimit;
    
    /** Determines if we should try and determine the symbol of the bin */
    bool m_runSymbolDetector;

    /** Determines whether or not we should write symbol images to disk */
    bool m_logSymbolImages;

    /** Each BGR value is the percent of total original value (for masking) */
    Image* m_percents;

    /** The input image where all white is white and everything else is black*/
    Image* m_whiteMaskedFrame;

    /** The input image where all black is white and everything else is black*/
    Image* m_blackMaskedFrame;

    /** The input image where all red is white and everything else is black*/
    Image* m_redMaskedFrame;

    /** Buffer we use to extract poritions of the image into
     *  @note Its always just a bit bigger then the raw image
     */
    unsigned char* m_extractBuffer;

    /** Buffer we use during image processing
     *  @note Its always just a bit bigger then the raw image
     */
    unsigned char* m_scratchBuffer1;

    /** Buffer we use during image processing
     *  @note Its always just a bit bigger then the raw image
     */
    unsigned char* m_scratchBuffer2;

    /** Buffer we use during image processing
     *  @note Its always just a bit bigger then the raw image
     */
    unsigned char* m_scratchBuffer3;
    
    /** Minimum percent for the white mask */
    int m_whiteMaskMinimumPercent;
    
    /** Minimum intensity of a pixel for it to be considered white */
    int m_whiteMaskMinimumIntensity;

    /** Treat red as black */
    bool m_blackIsRed;
    
    /** Minimum percent for the white mask */
    int m_blackMaskMinimumPercent;

    /** Maximum intensity of a pixel for it to be considered black */
    int m_blackMaskMaxTotalIntensity;        

    /** Erosion iterations on the red filtered image */
    int m_redErodeIterations;

    /** Dilation iterations on the red filtered image */
    int m_redDilateIterations;
    
    int m_redOpenIterations;
    int m_redCloseIterations;

    /** Minimum percent of the total pixel value for red */
    int m_redMinPercent;

    /** Minimum value of Red pixel value for for red */
    int m_redMinRValue;

    /** Maximum value of Green pixel value for for red */
    int m_redMaxGValue;

    /** Maximum value of Blue pixel value for for red */
    int m_redMaxBValue;
    
    /** Minimum pixel count of a black blob */
    int m_blobMinBlackPixels;

    /** Minimum pixel count of a white blob */
    int m_blobMinWhitePixels;

    /** Size of min red blob as a percent of the smallest black bin blob */
    double m_blobMinRedPercent;

    /** The maximum aspect ratio a blob can have and still be called a bin */
    double m_binMaxAspectRatio;

    /** Minimum percentage a bin has to be filled to be a bin */
    double m_binMinFillPercentage;

    /** The minimum amount of the black bob that must be filled to be a bin */
    double m_minRedFillPercentage;

    /** The maximum number of bin overlaps aloud */
    int m_binMaxOverlaps;
    
    /** The max distance between bins on different frames */
    double m_binSameThreshold;

    /** The number of frames something must be lost before we report it */
    int m_binLostFrames;
    
    /** Pixel resolution for hough based bin angle detection */
    int m_binHoughPixelRes;

    /** Threshold for hough based bin angle detection */
    int m_binHoughThreshold;

    /** Minimum length of lines found when detemining bin angle */
    int m_binHoughMinLineLength;

    /** Maximum gap between lines for them to be joined */
    int m_binHoughMaxLineGap;
    
    /** The ID of the next new bin to be found */
    int m_binID;

    /** LCH based filter for white */
    ColorFilter* m_whiteFilter;

    /** LCH based filter for black */
    ColorFilter* m_blackFilter;

    /** LCH based filter for red */
    ColorFilter* m_redFilter;

    /** Temporary LCH Image */
    OpenCVImage* m_frame;

	/**Kate changes*/
	void DetectorContours(Image* input);
	void getSquareBlob(cv::Mat erosion_dst, bincontours* bins, int  numberoftrackedcontours);
	int FindMatches(cv::Mat image, double* avgDistance, cv::Mat* descriptors_object);
	void calcTrainingData(void);
	int getTrainingData(cv::Mat* descriptors_object);
	void saveTrainingImages(cv::Mat* finalresize);
	void publishFoundEventSURF(bincontours bin);
	void publishLostEvent(Symbol::SymbolType color);
	void checkPreviousFrames();
	void publishFoundEventSURFAll();
	int m_threshbinary;
	cv::vector<cv::Point> m_ref_contours[4];
	cv::RotatedRect m_ref_box[4];
	void LoadReferences(void);

	cv::Mat img_whitebalance;
	//cv::Mat img_saturation;
	bincontours m_bin;
	//for surf featurs
	int m_minHessian; //used for finding keypoints
	std::string m_binyml;

	int m_numberofclasses; //four different bins
	int m_numberoftrainingimages; //number of training images PER CLASS - so a total of 40 images
	std::string m_filepath; //where the training iamges are located
	std::string m_filetype; //type of image file example ".png"
	std::string m_underscore; //just an underscore "_"
	std::string m_D;
	bool m_calcTraining;
	bool m_comparebins;
	bool m_saveimages;
	std::string m_trainingpath; //path to where teh images (not quite training images) will be saved
	double m_upperlimit; //upper threshold for bin detection

	bool m_BinoutlineFound;
	bool m_Bin37Found;
	bool m_Bin98Found;
	bool m_Bin10Found;
	bool m_Bin16Found;
	bool m_BinoutlineFoundBefore;
	bool m_Bin37FoundBefore;
	bool m_Bin98FoundBefore;
	bool m_Bin10FoundBefore;
	bool m_Bin16FoundBefore;
	int m_minSize;
	double m_minAspectRatio;	
	double m_maxAspectRatio;
	int m_maxSize;

	//trying to keep teh bins at the same angle as the main

	finalBins m_allbins; //data structure of all relevant data plus a little bit more
	int m_bin16frames;
int m_bin37frames;
int m_bin10frames;
int m_bin98frames;



	double m_minanglepercent; //min angle the internal bins can be, not really used
	double m_maxanglepercent; //not really used
	
	finalBins m_previousbins;
	int m_minAssumeFrame;
	int m_maxdistanceX;
	int m_maxdistanceY;
	int m_trainingsuccess;
 	cv::Mat m_descriptors_object[16*50];
int m_framecount;

 
};

} // namespace vision
} // namespace ram

#endif // RAM_BIN_DETECTOR_H_06_23_2007

/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Steven Friedman <sfriedm1@umd.edu>
 * All rights reserved.
 *
 * Author: Steven Friedman <sfriedm1@umd.edu>>
 * File:  packages/vision/test/src/GenColorFilterLookup.cpp
 */

// STD Includes
#include <assert.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

// Library Includes
#include "cv.h"
#include "highgui.h"
#include "boost/filesystem.hpp"
#include "boost/foreach.hpp"

// Project Includes
#include "math/include/SphericalPrimitive.h"
#include "math/include/IPrimitive3D.h"
#include "math/include/ImplicitSurface.h"
#include "core/include/BitField3D.h"
#include "vision/include/TableColorFilter.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Image.h"

int findElement(std::string ele, std::vector<std::string> list)
{
    for (unsigned int i=0; i<list.size(); i++)
        if (list.at(i).compare(ele) == 0) 
            return i;
    return -1;

}

cv::Mat buildTrainingSet(std::vector<cv::Mat> &images, 
                         std::vector<cv::Mat> &masks,
                         std::vector<std::string> &imageNames, 
                         std::vector<std::string> &maskNames)
{

    std::vector<int>::iterator it;
    std::vector<cv::Vec3b> trainingSet;
      
    for (unsigned int i=0; i<images.size(); i++)
    {
        std::string imgName = imageNames.at(i);
        int maskLoc = findElement(imgName, maskNames);
        if (maskLoc == -1)
            std::cout << "Error - No Matching Mask Name" << std::endl;

	std::cout << "making headers" << std::endl;
        cv::Mat img = images[i];
        cv::Mat mask = masks[maskLoc];
	std::cout << "made headers" << std::endl;

        cv::Vec3b temp;
        for(int i = 0; i < img.rows; i++)
        {
            for(int j = 0; j < img.cols; j++)
            {
                temp = mask.at<cv::Vec3b>(i,j);
                if ( temp[0] > 250 && temp[1] > 250 && temp[2] > 250 ) { // 255 means mask is on
                    trainingSet.push_back(img.at<cv::Vec3b>(i,j));                                
                }
            }
        }
    }

    cv::Mat ts = cvCreateMat(trainingSet.size(), 1, CV_32FC3); 
    size_t i = 0;
    BOOST_FOREACH(cv::Vec3b pixel, trainingSet)
    {
        ts.at<cv::Vec3f>(i, 0)[0] = pixel[0];
        ts.at<cv::Vec3f>(i, 0)[1] = pixel[1];
        ts.at<cv::Vec3f>(i, 0)[2] = pixel[2];
        i++;
    }
    return ts;
}


void loadImages(char* imgPath, std::vector<cv::Mat> &images, 
                std::vector<std::string> &imageNames, bool isMask = false)
{
    namespace fs = boost::filesystem;
    fs::path full_path(fs::initial_path<fs::path>() );

    full_path = fs::system_complete( fs::path( imgPath ) );
    
    fs::directory_iterator end_iter;
    
    if ( fs::is_directory( full_path ) ) { // this can be assumed
        
        for ( fs::directory_iterator dir_itr( full_path );
            dir_itr != end_iter;
            ++dir_itr )
        {

            if ( fs::is_regular_file( dir_itr->status() ) ) {
                imageNames.push_back( dir_itr->path().filename() );
                std::cout << "Reading: " << dir_itr->string() << std::endl;
		ram::vision::OpenCVImage *loadedImage = 
		  new ram::vision::OpenCVImage(dir_itr->string(),
					   ram::vision::Image::PF_BGR_8);
		if(!isMask)
		  {
		    // loadedImage->setPixelFormat(ram::vision::Image::PF_RGB_8);
		    // loadedImage->setPixelFormat(ram::vision::Image::PF_LCHUV_8);
		  }

		// IplImage *ipl = loadedImage->asIplImage();
		// std::cout << "w: " << ipl->width << "\n"
		// 	  << "h: " << ipl->height << std::endl;
		  
		cv::Mat img(loadedImage->asIplImage(), true);
		// std::cout << "matw: " << img.size().width << "\n"
		// 	  << "math: " << img.size().height << std::endl;

		delete loadedImage;
		
                images.push_back(img);
            }
        }
    }
}

void solveRadius(cv::Mat &labels, cv::Mat &center, 
                 cv::Mat &trainingSet, double *radius)
{
    for (int i = 0; i < center.rows; i++)
        radius[i] = 0;

    for(int i = 0; i < labels.size().height; i++)
    {
        int cIndex = labels.at<int>(i,0);

        double xCenter = center.at<cv::Vec3f>(cIndex,0)[0];
        double yCenter = center.at<cv::Vec3f>(cIndex,0)[1];
        double zCenter = center.at<cv::Vec3f>(cIndex,0)[2];

        double x = trainingSet.at<cv::Vec3f>(i,0)[0];
        double y = trainingSet.at<cv::Vec3f>(i,0)[1];
        double z = trainingSet.at<cv::Vec3f>(i,0)[2];

        double rad = sqrt(pow(x - xCenter, 2) + 
                          pow(y - yCenter, 2) + 
                          pow(z - zCenter, 2));

        if (radius[cIndex] < rad)
            radius[cIndex] = rad;

    }
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Try -h for help" << std::endl;;
        return 0;
    } else if (strcmp(argv[1], "-h") == 0) {
        std::cout << "arg1 is image folder" << std::endl;
        std::cout << "arg2 is mask folder" <<  std::endl;
        std::cout << "arg3 is serial data path" << std::endl;
        std::cout << "arg4 is blending factor (double)" << std::endl;
        std::cout << "please specify full path for everything" << std::endl;
        std::cout << "image and corresponding mask must have same name" << std::endl;
        return 0;
    }

    // Initialize and load images
    std::vector<cv::Mat> images;
    std::vector<cv::Mat> masks;
    std::vector<std::string> imageNames;
    std::vector<std::string> maskNames;

    std::cout << "Loading Images" << std::endl;
    loadImages(argv[1], images, imageNames);
    loadImages(argv[2], masks, maskNames, true);
    
    std::cout << "Building Training Set" << std::endl;
    cv::Mat trainingSet = buildTrainingSet(images, masks, 
                                           imageNames, maskNames);
    
    int numClusters = 50;
    std::cout << "Running KMeans" << std::endl;
    // Initializing variables for kmeans
    cv::Mat labels;
    cv::Mat center(numClusters, 1, CV_8UC3);
//commenting out because it through a hissy fit when I'm trying to get a Opencv2.4.5 workin
   // cv::kmeans(trainingSet, numClusters, labels, cv::TermCriteria(cv::TermCriteria::MAX_ITER, 50, 1),5, cv::KMEANS_RANDOM_CENTERS, &center);
    
    std::ofstream tss;
    tss.open("trainingSet");
    for(int i = 0; i < trainingSet.size().height; i++)
    {
        tss << trainingSet.at<cv::Vec3f>(i,1)[0] << ", "
            << trainingSet.at<cv::Vec3f>(i,1)[1] << ", "
            << trainingSet.at<cv::Vec3f>(i,1)[2] << "\n";
    }
    tss.close();

    std::ofstream cs;
    cs.open("centers");
    for(int i = 0; i < numClusters; i++)
    {
        cs << center.at<cv::Vec3f>(i,1)[0] << ", "
           << center.at<cv::Vec3f>(i,1)[1] << ", "
           << center.at<cv::Vec3f>(i,1)[2] << "\n";
    }
    cs.close();

    std::cout << "Calculating Primitives" << std::endl;        
    // Solving for radius
    double radius[center.rows];
    solveRadius(labels, center, trainingSet, radius);

    // for(int i = 0; i < center.rows; i++)
    //     std::cout << "Radius: " << radius[i] << std::endl;

    // Forming SphericalPrimitive vector
    namespace math = ram::math;
    // std::vector<math::SphericalPrimitive> sp; //not needed?
    std::vector<math::IPrimitive3DPtr> primitives;
    for(int i=0; i < center.rows; i++)
    {
        math::Vector3 cent = math::Vector3(center.at<cv::Vec3f>(i,0)[0],
                                           center.at<cv::Vec3f>(i,0)[1],
                                           center.at<cv::Vec3f>(i,0)[2]);
        primitives.push_back(
            math::IPrimitive3DPtr(
                new math::SphericalPrimitive(cent, radius[i])));
    }

    // Form ImplicitSurface
    double blendingFactor = atof(argv[4]); 
    
    std::cout << "Generating Implicit Surface" << std::endl;
    math::ImplicitSurface iSurface = 
        math::ImplicitSurface(primitives, blendingFactor);

    // Creating and Saving lookup table
    ram::vision::TableColorFilter::
        createLookupTable(argv[3], iSurface);
/*
    std::cout << "Testing" << std::endl;
    // Loading Lookup table for testing
    ram::vision::TableColorFilter tcf = 
        ram::vision::TableColorFilter(argv[3]);
    
    ram::vision::Image *input = 
        new ram::vision::OpenCVImage(argv[4],
            ram::vision::Image::PF_BGR_8);
    
    ram::vision::Image *output = new ram::vision::OpenCVImage();
    output->copyFrom(input);

    tcf.filterImage(input, output);
    
    IplImage *img = output->asIplImage();

    // Displaying and Saving filtered test image
    cvNamedWindow("input", CV_WINDOW_AUTOSIZE); 
    cvShowImage("input", input->asIplImage());

    cvNamedWindow("myWindow", CV_WINDOW_AUTOSIZE); 
    cvMoveWindow("myWindow", 300, 100);
    cvSaveImage(argv[5], img);
    cvShowImage("myWindow", img);
    
    cvWaitKey(0);
    
  */  
    return 0;
}


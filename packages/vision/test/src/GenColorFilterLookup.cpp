/*
Steven Friedman

arg1 is image folder
arg2 is mask folder
arg3 is serial data path
arg4 is input image path
arg5 is save image path
image and corresponding mask must have same name
 */

// STD Includes
#include <assert.h>
#include <iostream>
#include <vector>
#include <string>

// Library Includes
#include "cv.h"
#include "highgui.h"
#include "boost/filesystem.hpp"

// Project Includes
#include "math/include/SphericalPrimitive.h"
#include "math/include/IPrimitive3D.h"
#include "math/include/ImplicitSurface.h"
#include "core/include/BitField3D.h"
#include "vision/include/TableColorFilter.h"
#include "vision/include/OpenCVImage.h"

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
    std::vector<cv::Vec3b> trainingSet(1);
      
    for (unsigned int i=0; i<images.size(); i++)
    {
        std::string imgName = imageNames.at(i);
        int maskLoc = findElement(imgName, maskNames);
        if (maskLoc == -1)
            std::cout << "Error - No Matching Mask Name" << std::endl;
        
        cv::Mat img = images.at(i);
        cv::Mat mask = masks.at(maskLoc);
        
        //IplImage m = mask;
        //cvNamedWindow("Window", CV_WINDOW_AUTOSIZE); 
        //cvMoveWindow("Window", 100, 100);
        //cvShowImage("Window", &mask);
        //cvWaitKey(0);
        cv::Vec3b temp;
        for(int i=0; i<img.rows; i++)
        {
            for(int j=0; j<img.cols; j++)
            {
                temp = mask.at<cv::Vec3b>(i,j);
                if ( temp[0] > 250 && temp[1] > 250 && temp[2] > 250 ) { // 255 means mask is on
                    trainingSet.push_back(img.at<cv::Vec3b>(i,j));                                
                }
            }
        }
    }
    cv::Mat ts = cvCreateMat(trainingSet.size(), 3, CV_32FC3); 
    cv::Vec3b temp;
    for ( unsigned int i=0; i<trainingSet.size()*3; i+=3)
    {
        temp = trainingSet.at(i); 
        ts.data[i] = temp[0];
        ts.data[i+1] = temp[1];
        ts.data[i+2] = temp[2];
    
    }
    return ts;
}


void loadImages(char* imgPath, std::vector<cv::Mat> &images, 
                std::vector<std::string> &imageNames)
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
                cv::Mat img = cv::imread( dir_itr->path().filename() );
                images.push_back(img);
            }
        }
    }
}

void solveRadius(cv::Mat &labels, cv::Mat &center, 
                 cv::Mat &trainingSet, double *radius)
{
    for (int i=0; i<center.rows; i++)
        radius[i] = 0;

    unsigned int size = trainingSet.rows * trainingSet.cols;
    unsigned char *ldata = labels.data;
    unsigned char *cdata = center.data;
    unsigned char *tdata = trainingSet.data;
    unsigned char xcent, ycent, zcent, x, y, z;
    int cIndex;
    double rad;
    
    for(unsigned int i=0; i<size; i++)
    {
        cIndex = ldata[i];
        xcent = cdata[cIndex*3]; 
        ycent = cdata[cIndex*3+1];
        zcent = cdata[cIndex*3+2];
        x = tdata[i];
        y = tdata[i+1];
        z = tdata[i+2];

        rad = sqrt( pow(x-xcent, 2) + pow(y-ycent, 2) + pow(z-zcent, 2) );
        
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
        std::cout << "arg4 is input image path" << std::endl;
        std::cout << "arg5 is save image path" << std::endl;
        std::cout << "please specify full path for everything" << std::endl;
        std::cout << "image and corresponding mask must have same name" << std::endl;
        return 0;
    }

    // Initialize and load images
    std::vector<cv::Mat> images(10);
    std::vector<cv::Mat> masks(10);
    std::vector<std::string> imageNames(10);
    std::vector<std::string> maskNames(10);

    loadImages(argv[1], images, imageNames);
    loadImages(argv[2], masks, maskNames);
    
    cv::Mat trainingSet = buildTrainingSet(images, masks, 
                                           imageNames, maskNames);
 
    // Initializing variables for kmeans
    cv::Mat labels;
    cv::Mat center(50, 3, CV_32FC3);
    cv::kmeans(trainingSet, 50, labels, cv::TermCriteria(CV_TERMCRIT_ITER, 5000, 1), 
               5, cv::KMEANS_RANDOM_CENTERS, &center);
    
    // Solving for radius
    double radius[center.rows];
    solveRadius(labels, center, trainingSet, radius);
    
    // Forming SphericalPrimitive vector
    namespace math = ram::math;
    // std::vector<math::SphericalPrimitive> sp; //not needed?
    std::vector<math::IPrimitive3DPtr> primitives;
    for(int i=0; i<center.rows; i++)
    {
        math::Vector3 cent = math::Vector3(center.data[i],
                                           center.data[i+1],
                                           center.data[i+2]);

        primitives.push_back(
            math::IPrimitive3DPtr(
                new math::SphericalPrimitive( cent, radius[i] ) ) );
    }

    // Form ImplicitSurface
    double blendingFactor = 1.0; 
    
    math::ImplicitSurface iSurface = 
        math::ImplicitSurface(primitives, blendingFactor);

    // Creating and Saving lookup table
    ram::vision::TableColorFilter::
        createLookupTable(argv[3], iSurface);

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
    cvNamedWindow("myWindow", CV_WINDOW_AUTOSIZE); 
    cvMoveWindow("myWindow", 100, 100);
    cvSaveImage(argv[5], img);
    cvShowImage("myWindow", img);
    
    cvWaitKey(0);
    
    
    return 0;
}


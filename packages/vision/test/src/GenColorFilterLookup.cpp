/*
Steven Friedman

arg1 is image folder
arg2 is mask folder

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


        for(int i=0; i<img.rows; i++)
        {
            for(int j=0; j<img.cols; j++)
            {
                cv::Vec3b temp = mask.at<cv::Vec3b>(i,j);
                if ( mask.at<cv::Vec3b>(i,j)[0] < 250 ) { // 255 means mask is on?
                    trainingSet.push_back(img.at<cv::Vec3b>(i,j));                                
                }
            }
        }
    }
    cv::Mat ts = cvCreateMat(trainingSet.size(), 3, CV_32FC3); // 3 channel, 8 bit unsigned
    
    for ( unsigned int i=0; i<trainingSet.size(); i+=3)
    {
        cv::Vec3b temp = trainingSet.at(i); 
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
    // Initialize and load images
    std::vector<cv::Mat> images(10);
    std::vector<cv::Mat> masks(10);
    std::vector<std::string> imageNames(10);
    std::vector<std::string> maskNames(10);

    loadImages(argv[1], images, imageNames);
    loadImages(argv[2], masks, maskNames);
    
    cv::Mat trainingSet = buildTrainingSet(images, masks, 
                                           imageNames, maskNames);
 
    std::cout << "Check1" << std::endl;
    // Initializing variables for kmeans
    cv::Mat labels;// = cvCreateMat(trainingSet.rows, 1, CV_8UC1); 
    cv::Mat center(50, 3, CV_32FC3);// = cvCreateMat(50, 3, CV_8UC3);
    cv::kmeans(trainingSet, 50, labels, cv::TermCriteria(CV_TERMCRIT_ITER, 500, 1), 
               5, cv::KMEANS_RANDOM_CENTERS, &center);
    
    std::cout << "Check2" << std::endl;
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
        // sp.push_back(math::SphericalPrimitive(cent, radius.at(i))); //not needed?        
    }

    std::cout << "Check3" << std::endl;
    // Form ImplicitSurface
    double blendingFactor = 1.0; // Whats a blending factor?
    
    math::ImplicitSurface iSurface = 
        math::ImplicitSurface(primitives, blendingFactor);



    // Generate Lookup Table
    ram::core::BitField3D lookupTable = ram::core::BitField3D(256, 256, 256); 

    
    std::cout << "check4" << std::endl;
    double c;
    for(int c1 = 0; c1 < 256; c1++) {
        std::cout << "check5" << std::endl;
        for(int c2 = 0; c2 < 256; c2++) {
            for(int c3 = 0; c3 < 256; c3++) {
                //std::cout << "check6" << std::endl;
                c = iSurface.implicitFunctionValue(math::Vector3(c1, c2, c3));
                if ( c < 1)
                    lookupTable(c1, c2, c3) = true;
                else
                    lookupTable(c1, c2, c3) = false;
                //std::cout << "check6" << std::endl;
            }
        }
    }
    std::cout << "Check7" << std::endl;

    // Saving Lookup Table
    ram::vision::TableColorFilter tcf = ram::vision::TableColorFilter("a");
    std::cout << "Check8" << std::endl;
    tcf.saveLookupTable("/home/steven/stuff/testsave.serial", lookupTable);
    return 0;
}


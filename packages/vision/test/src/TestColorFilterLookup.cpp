/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Steven Friedman <sfriedm1@umd.edu>
 * All rights reserved.
 *
 * Author: Steven Friedman <sfriedm1@umd.edu> 
 * File:  packages/vision/test/src/testColorFilterLookup.cpp
 */

// STD Includes
#include <iostream>
#include <string>

// Library Includes
#include "cv.h"
#include "highgui.h"
#include "boost/filesystem.hpp"

// Project Includes
#include "vision/include/TableColorFilter.h"
#include "vision/include/OpenCVImage.h"

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Try -h for help" << std::endl;;
        return 0;
    } else if (strcmp(argv[1], "-h") == 0) {
        std::cout << "arg1 is serial data path" << std::endl;
        std::cout << "arg2 is input image path" << std::endl;
        std::cout << "arg3 is save image path" << std::endl;
        std::cout << "please specify full path for everything" << std::endl;
        return 0;
    }

    // Loading Input Images
    namespace fs = boost::filesystem;
    fs::path full_path(fs::initial_path<fs::path>() );

    full_path = fs::system_complete( fs::path( argv[2] ) );

    fs::directory_iterator end_iter;

    std::cout << "Testing" << std::endl;

    // Loading Lookup table for testing
    ram::vision::TableColorFilter tcf =
        ram::vision::TableColorFilter(argv[1]);

    for ( fs::directory_iterator dir_itr( full_path );
        dir_itr != end_iter;
        ++dir_itr )
    {
        if ( fs::is_regular_file ( dir_itr->status() ) ) {
            std::cout << "Reading: " << dir_itr->string() << std::endl;
        
            ram::vision::Image *input =
                new ram::vision::OpenCVImage(dir_itr->string().c_str(),
                    ram::vision::Image::PF_BGR_8);
    
            ram::vision::Image *output = new ram::vision::OpenCVImage();
            output->copyFrom(input);
    
            tcf.filterImage(input, output);
            
            IplImage *img = output->asIplImage();
            
            // Saving filtered test image
            size_t last = dir_itr->string().find_last_of("/");
            std::string fileName = dir_itr->string().substr(last+1);
            cvSaveImage((std::string(argv[3]).append(fileName)).c_str(),
                         img);
        }
    }
    return 0;
}

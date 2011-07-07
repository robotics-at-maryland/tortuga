/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vision/src/TableColorFilter.cpp
 */

// STD Includes
#include <iostream>
#include <fstream>
// Library Includes
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

// Project Includes
#include "vision/include/TableColorFilter.h"

namespace ram {
namespace vision {

TableColorFilter::TableColorFilter(std::string filepath) :
    m_filterTable(256u, 256u, 256u),
    m_propertySet(core::PropertySetPtr()),
    m_filepath(filepath)
{
    if(!loadLookupTable())
        assert(false && "lookup table could not be loaded");
}


void TableColorFilter::saveLookupTable(std::string filepath, core::BitField3D &filterTable)
{
    std::ofstream ofs(filepath.c_str());
    
    core::BitField3D bf(256u, 256u, 256u);
    {
        boost::archive::binary_oarchive oa(ofs);
        oa << filterTable;
    }
}

bool TableColorFilter::loadLookupTable()
{
    std::ifstream ifs(m_filepath.c_str());
    {
        boost::archive::binary_iarchive ia(ifs);
        ia >> m_filterTable;
    }
    return true;
}

void TableColorFilter::createLookupTable(std::string filepath, 
                                         math::ImplicitSurface &iSurface)
{
    static core::BitField3D bf(256u, 256u, 256u);
    double c;
    
    std::ofstream surfStream;
    surfStream.open("implicitSurface");

    std::ofstream bfStream;
    bfStream.open("bitField");

    for(int c1 = 0; c1 < 256; c1++) {
        //std::cout << c1 << std::endl;
        for(int c2 = 0; c2 < 256; c2++) {
            for(int c3 = 0; c3 < 256; c3++) {
                c = iSurface.implicitFunctionValue(math::Vector3(c1, c2, c3));
                surfStream << c1 << ", " << c2 << ", " << c3 << ", " << c << "\n";
                if ( c < 1)
                {
                    bf(c1, c2, c3) = true;
                    bfStream << c1 << ", " << c2 << ", " << c3 << "\n";
                }
                else
                {
                    bf(c1, c2, c3) = false;
                }
            }
        }
    }
    
    saveLookupTable(filepath, bf);
}

void TableColorFilter::filterImage(Image* input, Image* output)
{
    int numPixels = input->getWidth() * input->getHeight();
    int nChannels = 0;
    unsigned char *inputData = input->getData();
    unsigned char *outputData = NULL;
    if(output)
    {
        outputData = output->getData();
        nChannels = output->getNumChannels();
    }
    else
    {
        outputData = input->getData();
        nChannels = input->getNumChannels();
    }

    for(int i = 0; i < numPixels; ++i)
    {
        unsigned char result = m_filterTable(
            *inputData, *inputData + 1, *inputData + 2);

        for(int k = 0; k < nChannels; k++, outputData++)
        {
            if(result)
                *outputData = 255;
            else
                *outputData = 0;
        }

        inputData += 3;
    }
}

void TableColorFilter::inverseFilterImage(Image* input, Image* output)
{
    int numPixels = input->getWidth() * input->getHeight();
    int nChannels = 0;
    unsigned char *inputData = input->getData();
    unsigned char *outputData = NULL;
    if(output)
    {
        outputData = output->getData();
        nChannels = output->getNumChannels();
    }
    else
    {
        outputData = input->getData();
        nChannels = input->getNumChannels();
    }

    for(int i = 0; i < numPixels; ++i)
    {
        unsigned char result = !m_filterTable(
            *inputData, *inputData + 1, *inputData + 2);

        for(int k = 0; k < nChannels; k++, outputData++)
        {
            if(result)
                *outputData = 0;
            else
                *outputData = 255;
        }

        inputData += 3;
    }
}

} // namespace vision
} // namespace ram

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
    //if(!loadLookupTable())
    //    assert(false && "lookup table could not be loaded");
}


void TableColorFilter::saveLookupTable(std::string filepath, core::BitField3D filterTable)
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

void TableColorFilter::createLookupTable(std::string filepath)
{

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
            *outputData = 255 * result;

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
            *outputData = 255 * result;

        inputData += 3;
    }
}


} // namespace vision
} // namespace ram

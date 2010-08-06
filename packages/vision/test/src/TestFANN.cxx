/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 David Love
 * All rights reserved.
 *
 * Author: David Love <loved@umd.edu>
 * File:  packages/vision/test/src/TestFANNSymbolDetector.cpp
 */

// STD Includes
#include <vector>
#include <sstream>

// Library Includes
#include <UnitTest++/UnitTest++.h>

//#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

// Project Includes
#include "core/include/ConfigNode.h"

#include "vision/include/OpenCVImage.h"
#include "vision/include/FANNTrainer.h"
#include "vision/include/FANNSymbolDetector.h"

#include "vision/test/include/Utility.h"



using namespace ram;
namespace bf = boost::filesystem;

/*
static boost::filesystem::path getImagesDir()
{
    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
    return root / "packages" / "vision" / "test" / "data" / "images" /
        "testfann";
}

static std::string getSymbolNetworkFile()
{
    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
    return (root / "packages" / "vision" / "test" / "data" /
            "symbols.irn").file_string();
}
*/

class ColorDetector : public vision::FANNSymbolDetector
{
public:
    ColorDetector(std::string cfgStr) :
        vision::FANNSymbolDetector(3, 3,
            core::ConfigNode::fromString(cfgStr))
    {}

    vision::Symbol::SymbolType getSymbol() { return vision::Symbol::UNKNOWN; }

    virtual bool needSquareCropped() { return false; }
    
    virtual void processImage(ram::vision::Image* input, ram::vision::Image*)
    {    runNN(input); }
    
    virtual void getImageFeatures(vision::Image* inputImage, float* features)
    {
        // Get the average pixel values
        double R, B, G;
        vision::Image::getAveragePixelValues(inputImage, 0, 0, 99, 99,
                                             B, G, R);

        // Assigne them to our output features (after normalizing)
        features[0] = (float)R/255.0;
        features[1] = (float)G/255.0;
        features[2] = (float)B/255.0;
    }
};


struct FANNFixture
{
    FANNFixture()
    {
        std::stringstream ss;
        ss << "FANNTestNueralNetwork" << "_" << vision::getPid() << ".irn";
        fileName = ss.str();
    }
    
    ~FANNFixture()
    {
        BOOST_FOREACH(vision::Image* image, yellowImages)
            delete image;
        BOOST_FOREACH(vision::Image* image, purpleImages)
            delete image;
        BOOST_FOREACH(vision::Image* image, tealImages)
            delete image;

        // Remove network file
        bf::path netFile(fileName);
        if (bf::exists(netFile))
            bf::remove(netFile);
    }
    
    void addImage(std::vector<vision::Image*>& images, unsigned char R,
                  unsigned char G, unsigned char B)
    {
        vision::Image* image = new vision::OpenCVImage(100, 100);
        vision::makeColor(image, R, G, B);
        images.push_back(image);
    }

    std::string fileName;
    
    std::vector<vision::Image*> yellowImages;
    std::vector<vision::Image*> purpleImages;
    std::vector<vision::Image*> tealImages;
};

SUITE(FANN) {
    
TEST_FIXTURE(FANNFixture, test)
{
    // Generate test images
    addImage(yellowImages, 235, 255, 0);
    addImage(yellowImages, 255, 232, 0);
    addImage(yellowImages, 255, 239, 0);
    addImage(yellowImages, 255, 215, 0);

    // Generate purple test images
    addImage(purpleImages, 235, 0, 255);
    addImage(purpleImages, 255, 0, 232);
    addImage(purpleImages, 255, 0, 239);
    addImage(purpleImages, 255, 0, 215);

    // Generate teal images
    addImage(tealImages, 0, 235, 255);
    addImage(tealImages, 0, 255, 232);
    addImage(tealImages, 0, 255, 239);
    addImage(tealImages, 0, 255, 215);

    // Train our nueral network
    vision::FANNSymbolDetectorPtr colorDetector(
        new ColorDetector("{ 'training' : 1 }"));
    vision::FANNTrainer trainer(colorDetector);

    // Load up the data
    FANN::training_data data;
    trainer.addTrainData(0, data, yellowImages);
    trainer.addTrainData(1, data, purpleImages);
    trainer.addTrainData(2, data, tealImages);

    // train the network
    trainer.runTraining (data);
    
    // save the network
    trainer.save(bf::path(fileName));

    // Now lets test the detector by creating one
    std::stringstream cfg;
    cfg << "{ 'nueralNetworkFile' : '"  << fileName << "'}";
    vision::FANNSymbolDetectorPtr trainedColorDetector(
        new ColorDetector(cfg.str()));

    // Reference images
    vision::Image* yellow = new vision::OpenCVImage(100, 100);
    vision::makeColor(yellow, 255, 255, 0);
    vision::Image* purple = new vision::OpenCVImage(100, 100);
    vision::makeColor(purple, 255, 0, 255);
    vision::Image* teal = new vision::OpenCVImage(100, 100);
    vision::makeColor(teal, 0, 255, 255);

    // Tests
    trainedColorDetector->processImage(yellow, 0);
    CHECK_EQUAL(0, trainedColorDetector->getResult());

    trainedColorDetector->processImage(purple, 0);
    CHECK_EQUAL(1, trainedColorDetector->getResult());

    trainedColorDetector->processImage(teal, 0);
    CHECK_EQUAL(2, trainedColorDetector->getResult());

    
    // Cleanup
    delete yellow;
    delete purple;
    delete teal;
}
    
} // SUITE(FANNSymbolDetector)

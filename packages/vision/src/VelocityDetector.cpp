/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Nate Davidge
 * All rights reserved.
 *
 * Author: Nate Davidge <ndavidge@umd.edu>
 * File:  packages/vision/include/VelocityDetector.h
 */

// Library Includes
#include "highgui.h"
#include <boost/foreach.hpp>
#include <fftw3.h>

// Project Includes
#include "vision/include/main.h"
#include "vision/include/Camera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/VelocityDetector.h"
#include "vision/include/Events.h"

#include "core/include/PropertySet.h"

#ifndef M_PI
#define M_PI 3.14159
#endif

namespace ram {
namespace vision {

VelocityDetector::VelocityDetector(core::ConfigNode config,
                                   core::EventHubPtr eventHub) :
    Detector(eventHub),
    m_currentFrame(new OpenCVImage(640, 480)),
    m_lastFrame(new OpenCVImage(640, 480))
{
    init(config);
}
    
void VelocityDetector::init(core::ConfigNode config)
{
    // Detection variables
    // NOTE: The property set automatically loads the value from the given
    //       config if its present, if not it uses the default value presented.
    core::PropertySetPtr propSet(getPropertySet());

    propSet->addProperty(config, false, "usePhaseCorrelation",
        "Run the phase correlation algorithim", false, &m_usePhaseCorrelation);
    
    propSet->addProperty(config, false, "phaseLineScale",
        "Scale red line draw by the phase correlation",
        1.0, &m_phaseLineScale, 1.0, 50.0);
    
    // Parameters for LK Flow
    propSet->addProperty(config, false, "useLKFlow",
                         "Run the phase pyramidal Lucas-Kanade algorithm", 
                         false, &m_useLKFlow);
    propSet->addProperty(config, false, "lkMaxNumberFeatures", 
                         "maximum number of features to track", 400,
                         &m_lkMaxNumberFeatures);
    propSet->addProperty(config, false, "lkMinQualityFeatures", 
                         "minimum quality of the features to track", .01,
                         &m_lkMinQualityFeatures);
    propSet->addProperty(config, false, "lkMinEucDistance", 
                         "minimum Euclidean distance between features", .01,
                         &m_lkMinEucDistance);
    propSet->addProperty(config, false, "lkIterations", 
                         "termination criteria (iterations)", 20, 
                         &m_lkIterations);
    propSet->addProperty(config, false, "lkEpsilon", 
                         "termination criteria (better than error)", .3, 
                         &m_lkEpsilon);

    // Initialize grey scale images (for PhaseCorrelation)
    m_currentGreyScale = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);
    m_lastGreyScale = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);
    m_phaseResult = cvCreateImage(cvSize(640, 480), IPL_DEPTH_64F, 1);
    
    // Initialize scratch images for LK
    m_eig_image = cvCreateImage(cvSize(640, 480), IPL_DEPTH_32F, 1);
    m_temp_image = cvCreateImage(cvSize(640, 480), IPL_DEPTH_32F, 1);
    m_pyramid1 = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);
    m_pyramid2 = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);
}
    
VelocityDetector::~VelocityDetector()
{
    delete m_currentFrame;
    delete m_lastFrame;

    cvReleaseImage(&m_currentGreyScale);
    cvReleaseImage(&m_lastGreyScale);
    cvReleaseImage(&m_phaseResult);
    cvReleaseImage(&m_eig_image);
    cvReleaseImage(&m_temp_image);
    cvReleaseImage(&m_pyramid1);
    cvReleaseImage(&m_pyramid2);
}
    
void VelocityDetector::processImage(Image* input, Image* output)
{
    // Resize images and data structures if needed
    if ((m_lastFrame->getWidth() != input->getWidth()) &&
    (m_lastFrame->getHeight() != input->getHeight()))
    { 
        // Release all the old images
        cvReleaseImage(&m_currentGreyScale);
        cvReleaseImage(&m_lastGreyScale);
        cvReleaseImage(&m_phaseResult);
        cvReleaseImage(&m_eig_image);
        cvReleaseImage(&m_temp_image);
        cvReleaseImage(&m_pyramid1);
        cvReleaseImage(&m_pyramid2);
        
        // Allocate all the new
        CvSize frameSize = cvSize(input->getWidth(), input->getHeight());
        m_currentGreyScale = cvCreateImage(frameSize, IPL_DEPTH_8U, 1);
        m_lastGreyScale = cvCreateImage(frameSize, IPL_DEPTH_8U, 1);
        m_phaseResult = cvCreateImage(frameSize, IPL_DEPTH_64F, 1);
        m_eig_image = cvCreateImage(frameSize, IPL_DEPTH_32F, 1);
        m_temp_image = cvCreateImage(frameSize, IPL_DEPTH_32F, 1);
        m_pyramid1 = cvCreateImage(frameSize, IPL_DEPTH_8U, 1);
        m_pyramid2 = cvCreateImage(frameSize, IPL_DEPTH_8U, 1);
    }

    // Copy the current frame locally
    m_currentFrame->copyFrom(input);

    if (0 != output)
    {
        // Copy the input image
        output->copyFrom(input);
    }

    // Now run all different optical flow algorithms
    if (m_usePhaseCorrelation)
        phaseCorrelation(output);
    else if (m_useLKFlow)
        LKFlow(output);

    // We are done with our work now last save the input for later use
    m_lastFrame->copyFrom(input);
}

math::Vector2 VelocityDetector::getVelocity()
{
    return m_velocity;
}

void VelocityDetector::usePhaseCorrelation()
{
    m_usePhaseCorrelation = true;
}

void VelocityDetector::useLKFlow()
{
    m_useLKFlow = true;
}
    
void VelocityDetector::LKFlow(Image* output)
{
    // Convert the current image to grey scale
    cvCvtColor(m_currentFrame->asIplImage(), m_currentGreyScale, CV_BGR2GRAY);
    
    
    // make it happen
    IplImage* last = m_lastGreyScale;
    IplImage* current = m_currentGreyScale;
        
    CvPoint2D32f frame1_features[m_lkMaxNumberFeatures];
    
    int number_of_features = m_lkMaxNumberFeatures;
    
    // Choosing the features to track (Shi-Tomasi)
    
    cvGoodFeaturesToTrack(last, m_eig_image, m_temp_image, frame1_features, 
                          &number_of_features, 
                          m_lkMinQualityFeatures, m_lkMinEucDistance, NULL);
    CvPoint2D32f frame2_features[m_lkMaxNumberFeatures];
    
    char optical_flow_found_feature[m_lkMaxNumberFeatures];
    float optical_flow_feature_error[m_lkMaxNumberFeatures];
    
    // To avoid "aperature problem"
    CvSize optical_flow_window = cvSize(3,3);
    
    // Terminates after iterations or when better epsilon is found
    CvTermCriteria optical_flow_termination_criteria
        = cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, m_lkIterations, m_lkEpsilon);
    
    // Running pyramidla L-K Optical Flow algorithm on the desired features
    cvCalcOpticalFlowPyrLK(last, current, m_pyramid1, 
                           m_pyramid2, 
                           frame1_features, frame2_features, 
                           number_of_features, optical_flow_window, 5, 
                           optical_flow_found_feature, 
                           optical_flow_feature_error, 
                           optical_flow_termination_criteria, 0);
    
    // Long complicated process for drawing the flow field

    // We are done copy current over to the last
    cvCopyImage(m_currentGreyScale, m_lastGreyScale);
    
    // needs to return m_velocity
}
        
void VelocityDetector::phaseCorrelation(Image* output)
{
    // Convert the current image to grey scale
    cvCvtColor(m_currentFrame->asIplImage(), m_currentGreyScale, CV_BGR2GRAY);

    // Run the phase correlation process
    {
    IplImage* ref = m_currentGreyScale;
    IplImage* tpl = m_lastGreyScale;
    IplImage* poc = m_phaseResult;

        int         i, j, k;
        double        tmp;
        
        /* get image properties */
        int width    = ref->width;
        int height   = ref->height;
        int step     = ref->widthStep;
        int fft_size = width * height;

        /* setup pointers to images */
        uchar         *ref_data = ( uchar* ) ref->imageData;
        uchar         *tpl_data = ( uchar* ) tpl->imageData;
        double         *poc_data = ( double* )poc->imageData;
        
        /* allocate FFTW input and output arrays */
        fftw_complex *img1 = ( fftw_complex* )fftw_malloc( sizeof( fftw_complex ) * width * height );
        fftw_complex *img2 = ( fftw_complex* )fftw_malloc( sizeof( fftw_complex ) * width * height );
        fftw_complex *res  = ( fftw_complex* )fftw_malloc( sizeof( fftw_complex ) * width * height );        
        
        /* setup FFTW plans */
        fftw_plan fft_img1 = fftw_plan_dft_1d( width * height, img1, img1, FFTW_FORWARD,  FFTW_ESTIMATE );
        fftw_plan fft_img2 = fftw_plan_dft_1d( width * height, img2, img2, FFTW_FORWARD,  FFTW_ESTIMATE );
        fftw_plan ifft_res = fftw_plan_dft_1d( width * height, res,  res,  FFTW_BACKWARD, FFTW_ESTIMATE );
        
        /* load images' data to FFTW input */
        for( i = 0, k = 0 ; i < height ; i++ ) {
                for( j = 0 ; j < width ; j++, k++ ) {
                        img1[k][0] = ( double )ref_data[i * step + j];
                        img1[k][1] = 0.0;
                        
                        img2[k][0] = ( double )tpl_data[i * step + j];
                        img2[k][1] = 0.0;
                }
        }
        
        /* obtain the FFT of img1 */
        fftw_execute( fft_img1 );
        
        /* obtain the FFT of img2 */
        fftw_execute( fft_img2 );
        
        /* obtain the cross power spectrum */
        for( i = 0; i < fft_size ; i++ ) {
                res[i][0] = ( img2[i][0] * img1[i][0] ) - ( img2[i][1] * ( -img1[i][1] ) );
                res[i][1] = ( img2[i][0] * ( -img1[i][1] ) ) + ( img2[i][1] * img1[i][0] );

                tmp = sqrt( pow( res[i][0], 2.0 ) + pow( res[i][1], 2.0 ) );

                res[i][0] /= tmp;
                res[i][1] /= tmp;
        }

        /* obtain the phase correlation array */
        fftw_execute(ifft_res);

        /* normalize and copy to result image */
        for( i = 0 ; i < fft_size ; i++ ) {
        poc_data[i] = res[i][0] / ( double )fft_size;
        }

        /* deallocate FFTW arrays and plans */
        fftw_destroy_plan( fft_img1 );
        fftw_destroy_plan( fft_img2 );
        fftw_destroy_plan( ifft_res );
        fftw_free( img1 );
        fftw_free( img2 );
        fftw_free( res );        
    }

    // Find the result and 
    CvPoint minloc, maxloc;
    double  minval, maxval;
    cvMinMaxLoc(m_phaseResult, &minval, &maxval, &minloc, &maxloc, 0);

    // Transform coordinates by moving the origin to the center, and 
    // swapping quadrants diagonally
    int quadrantWidth = m_currentFrame->getWidth() / 2;
    int quadrantHeight = m_currentFrame->getHeight() / 2;
    double outX = 0;
    double outY = 0;
    if ((maxloc.x < quadrantWidth) && (maxloc.y < quadrantHeight))
    {
        // Upper left quadrant
        outX = (double)maxloc.x;
    outY = - (double)maxloc.y;
    } 
    else if ((maxloc.x >= quadrantWidth) && (maxloc.y < quadrantHeight))
    {
        // Upper right quadrant
        outX = -((double)m_currentFrame->getWidth() - (double)maxloc.x);
    outY = - (double)maxloc.y;
    }
    else if ((maxloc.x < quadrantWidth) && (maxloc.y >= quadrantHeight))
    {
        // Lower left quadrant
        outX = (double)maxloc.x;
    outY = ((double)m_currentFrame->getHeight() - (double)maxloc.y);
    } 
    else if ((maxloc.x >= quadrantWidth) && (maxloc.y >= quadrantHeight))
    {
        // Lower right quadrant
        outX = -((double)m_currentFrame->getWidth() - (double)maxloc.x);
    outY = ((double)m_currentFrame->getHeight() - (double)maxloc.y);
    }
    else 
    {
        assert(false && "Should not be here");
    }
    
    // Assign velocity
    m_velocity = math::Vector2(outX, outY);

    if (output)
    {
        CvPoint start;
    start.x = output->getWidth() / 2;
    start.y = output->getHeight() / 2;
    CvPoint end;
    end.x = start.x + ((int)(outX*m_phaseLineScale));
    end.y = start.y - ((int)(outY*m_phaseLineScale));
    cvLine(output->asIplImage(), start, end, CV_RGB(255,0,0), 1, CV_AA, 0);
    }


    // We are done copy current over to the last
    cvCopyImage(m_currentGreyScale, m_lastGreyScale);
}
    
} // namespace vision
} // namespace ram

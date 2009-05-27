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

    /*    propSet->addProperty(config, false, "initialMinPixels",
        "Red pixel count required for blob to a buoy",
        400, &m_initialMinRedPixels);

    propSet->addProperty(config, false, "foundMinPixelScale",
        "Scale value for the buoy pixel count when its found",
        0.85, &m_foundMinPixelScale);*/

    // Initialize grey scale images
    m_currentGreyScale = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);
    m_lastGreyScale = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);
    m_phaseResult = cvCreateImage(cvSize(640, 480), IPL_DEPTH_64F, 1);
}
    
VelocityDetector::~VelocityDetector()
{
    delete m_currentFrame;
    delete m_lastFrame;
    cvReleaseImage(&m_currentGreyScale);
    cvReleaseImage(&m_lastGreyScale);
}
    
void VelocityDetector::processImage(Image* input, Image* output)
{
    // Resize images and data structures if needed
    if ((m_lastFrame->getWidth() != input->getWidth()) &&
	(m_lastFrame->getHeight() != input->getHeight()))
    { 
        cvReleaseImage(&m_currentGreyScale);
	cvReleaseImage(&m_lastGreyScale);
	m_currentGreyScale = cvCreateImage(cvSize(input->getWidth(),
						  input->getHeight()), 
					   IPL_DEPTH_8U, 1);
	m_lastGreyScale = cvCreateImage(cvSize(input->getWidth(),
					       input->getHeight()), 
					IPL_DEPTH_8U, 1);
	m_phaseResult = cvCreateImage(cvSize(input->getWidth(),
					     input->getHeight()), 
				      IPL_DEPTH_64F, 1);
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
        phasePhaseCorrelation(output);

    // We are done with our work now last save the input for later use
    m_lastFrame->copyFrom(input);
}

void VelocityDetector::phasePhaseCorrelation(Image* output)
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
        int width           = ref->width;
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

    // Transform coordinates properly (deal with odd quadrant issue)
    double outX = -1 * ((m_currentFrame->getWidth() / 2.0) - (double)maxloc.x);
    double outY = (m_currentFrame->getHeight() / 2.0) - (double)maxloc.y;
    outX *= -1;
    outY *= -1;

    if (output)
    {
        CvPoint start;
	start.x = output->getWidth() / 2;
	start.y = output->getHeight() / 2;
	//start.x = 0;
	//start.y = 0;
	CvPoint end;
	end.x = start.x + ((int)outX);
	end.y = start.x - ((int)outY);
	cvLine(output->asIplImage(), start, end, CV_RGB(255,0,0), 2, CV_AA, 0 );
    }


    // We are done copy current over to the last
    cvCopyImage(m_currentGreyScale, m_lastGreyScale);
}
    
} // namespace vision
} // namespace ram

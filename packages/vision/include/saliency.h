// Copyright (c) 2008, Sebastian Montabone
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the owner nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// 
// File:   saliency.h
// Author: Sebastian Montabone <samontab at puc.cl>
//
// Created on September 24, 2008, 2:57 PM
//
// Fine-grained Saliency library (FGS). 
// http://www.samontab.com/web/saliency/
// This library calculates fine grained saliency in real time using integral images.
// It requires OpenCV.
//

#ifndef _saliency_H
#define	_saliency_H


 
// Library Includes
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <cmath>
// Must be included last
#include "vision/include/Export.h"

using namespace std;
using namespace cv;

namespace ram {
namespace vision {


//class __declspec(dllexport) Saliency
class Saliency
{
public:
	Saliency();
	void calcIntensityChannel(IplImage *src, IplImage *dst);

private:
	void copyImage(IplImage *src, IplImage *dst);	
	void getIntensityScaled(IplImage * integralImage, IplImage * gray, IplImage *saliencyOn, IplImage *saliencyOff, int neighborhood);
	float getMean(IplImage * srcArg, CvPoint PixArg, int neighbourhood, int centerVal);
	void mixScales(IplImage **saliencyOn, IplImage *intensityOn, IplImage **saliencyOff, IplImage *intensityOff, const int numScales);
	void mixOnOff(IplImage *intensityOn, IplImage *intensityOff, IplImage *intensity);
	void getIntensity(IplImage *srcArg, IplImage *dstArg,  IplImage *dstOnArg,  IplImage *dstOffArg, bool generateOnOff);
}; //end class


}//end vision
} //end ram


#endif	/* _saliency_H */

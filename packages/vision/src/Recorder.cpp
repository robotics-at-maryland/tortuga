/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/Recorder.cpp
 */


// Library Includes
#include "cv.h"
#include "highgui.h"

// Project Includes
#include "vision/include/main.h"
#include "vision/include/Recorder.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Camera.h"


namespace ram {
namespace vision {

Recorder::Recorder(Camera* camera, const char* filename)
{
	cam = camera;
	frame = new OpenCVImage(640, 480);
	isColor = 1;
	fps     = 30;  // or 30
	frameW=640;
	frameH=480;
	FILE* video=fopen(filename,"w");
	//Yes, thats a comma, not a semicolon, it forces the compiler to close the file before attempting the next step, creating a video writer on a file that may not exist before the close finishes
	fclose(video),
	writer=cvCreateVideoWriter(filename,CV_FOURCC('D','I','V','X'),
				fps,cvSize(frameW,frameH),1);
}

Recorder::~Recorder()
{
	delete frame;
	cvReleaseVideoWriter(&writer);
}

void Recorder::update()
{
	cam->getImage(frame);
	IplImage* image =(IplImage*)(*frame);
	cvWriteFrame(writer,image);
}

void Recorder::writeFrame(IplImage* image)
{
	cvWriteFrame(writer,image);
}

} // namespace vision
} // namespace ram

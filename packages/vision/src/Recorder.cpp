#include "vision/include/Recorder.h"

using namespace std;
using namespace ram::vision;

Recorder::Recorder(OpenCVCamera* camera, const char* filename)
{
	cam = camera;
	frame = new ram::vision::OpenCVImage(640, 480);
	isColor = 1;
	fps     = 30;  // or 30
	frameW=480;
	frameH=640;
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



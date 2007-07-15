#include "cv.h"
#include "highgui.h"
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
	CvCapture* capture =NULL;
	CvCapture* captureDown=NULL;
	
	CvVideoWriter *writer = NULL;
	CvVideoWriter *writeDown =NULL;

void handler(int x)
{
	printf("Releasing Cameras");
	if (capture)
		cvReleaseCapture(&capture);
	if (captureDown)
		cvReleaseCapture(&captureDown);
	if (writer)
		cvReleaseVideoWriter(&writer);
	if (writeDown)
		cvReleaseVideoWriter(&writeDown);

	exit(0);
}

// A Simple Camera Capture Framework
int main() {

  capture = cvCaptureFromCAM(300);
  captureDown = cvCaptureFromCAM(301);
  
  signal(SIGINT,handler);
  //capture = cvCaptureFromCAM(CV_CAP_ANY+1);
  if( !capture ) {
    fprintf( stderr, "ERROR: capture is NULL \n" );
    getchar();
    return -1;
  }
  if (!captureDown)
	{
		fprintf( stderr, "ERROR: capture is NULL \n" );
		getchar();
		return -1;
	}
  //cvSetCaptureProperty(capture, CV_CAP_PROP_FOURCC, CV_CAP_PROP_FOURCC_YUV411);
  //cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, 30);

CvVideoWriter *writer = 0;
CvVideoWriter *writeDown =NULL;
  int isColor = 1;
  int fps     = 30;  // or 30
  int frameW  = 640; // 744 for firewire cameras
  int frameH  = 480; // 480 for firewire cameras
  
  /* Creates a file if none exists, also crashes without the comma*/
  FILE* video=fopen("out.avi","w");
  fclose(video);
  FILE* video2=fopen("out2.avi","w");
  fclose(video2),
  writer=cvCreateVideoWriter("out.avi",CV_FOURCC('D','I','V','X'),
                             fps,cvSize(frameW,frameH),1);

	writeDown=cvCreateVideoWriter("out2.avi",CV_FOURCC('D','I','V','X'),
							fps,cvSize(frameW,frameH),1);


  printf("Camera Capture Properties:\n");
  printf("\tWidth:  %f\n", cvGetCaptureProperty(capture,
                                                CV_CAP_PROP_FRAME_WIDTH));
  printf("\tHeight: %f\n", cvGetCaptureProperty(capture,
                                                CV_CAP_PROP_FRAME_HEIGHT));
  printf("\tFPS:    %f\n", cvGetCaptureProperty(capture,
                                                CV_CAP_PROP_FPS));
  
  // Create a window in which the captured images will be presented
  //cvNamedWindow( "Raw Camera Image", CV_WINDOW_AUTOSIZE );

  // Show the image captured from the camera in the window and repeat
  while( 1 ) {
    // Get one frame
    IplImage* frame = cvQueryFrame( capture );
    if( !frame ) {
      fprintf( stderr, "ERROR: frame is null...\n" );
      getchar();
      break;
    }
	IplImage* frameDown=cvQueryFrame(captureDown);
	if (!frameDown) {
	  fprintf( stderr, "ERROR: frameDown is null...\n" );
      getchar();
      break;
    }
	//cvShowImage( "Raw Camera Image", frame );

    cvWriteFrame(writer,frame);
	cvWriteFrame(writeDown,frameDown);

    // Do not release the frame!

    //If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
    //remove higher bits using AND operator
    if( (cvWaitKey(10) & 255) == 27 ) break;
  }

  // Release the capture device housekeeping
  cvReleaseCapture( &capture );
  cvReleaseCapture( &captureDown);
  cvReleaseVideoWriter(&writer);
  cvReleaseVideoWriter(&writeDown);
//  cvDestroyWindow( "Raw Camera Image" );
  return 0;
}

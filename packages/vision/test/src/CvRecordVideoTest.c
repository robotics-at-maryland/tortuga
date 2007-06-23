#include "cv.h"
#include "highgui.h"
#include <stdio.h>

// A Simple Camera Capture Framework
int main() {

  CvCapture* capture = cvCaptureFromCAM(CV_CAP_ANY);
  if( !capture ) {
    fprintf( stderr, "ERROR: capture is NULL \n" );
    getchar();
    return -1;
  }

  CvVideoWriter *writer = 0;
  int isColor = 1;
  int fps     = 10;  // or 30
  int frameW  = 640; // 744 for firewire cameras
  int frameH  = 480; // 480 for firewire cameras
  
  /* Creates a file if none exists, also crashes without the comma*/
  FILE* video=fopen("out.avi","w");
  fclose(video),
  writer=cvCreateVideoWriter("out.avi",CV_FOURCC('D','I','V','X'),
                             fps,cvSize(frameW,frameH),1);

  printf("Camera Capture Properties:\n");
  printf("\tWidth:  %f\n", cvGetCaptureProperty(capture,
                                                CV_CAP_PROP_FRAME_WIDTH));
  printf("\tHeight: %f\n", cvGetCaptureProperty(capture,
                                                CV_CAP_PROP_FRAME_HEIGHT));
  printf("\tFPS:    %f\n", cvGetCaptureProperty(capture,
                                                CV_CAP_PROP_FPS));
  
  // Create a window in which the captured images will be presented
  cvNamedWindow( "Raw Camera Image", CV_WINDOW_AUTOSIZE );

  // Show the image captured from the camera in the window and repeat
  while( 1 ) {
    // Get one frame
    IplImage* frame = cvQueryFrame( capture );
    if( !frame ) {
      fprintf( stderr, "ERROR: frame is null...\n" );
      getchar();
      break;
    }
    
    cvShowImage( "Raw Camera Image", frame );

    cvWriteFrame(writer,frame);

    // Do not release the frame!

    //If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
    //remove higher bits using AND operator
    if( (cvWaitKey(10) & 255) == 27 ) break;
  }

  // Release the capture device housekeeping
  cvReleaseCapture( &capture );
  cvReleaseVideoWriter(&writer);
  cvDestroyWindow( "Raw Camera Image" );
  return 0;
}

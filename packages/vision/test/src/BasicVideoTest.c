#include "cv.h"
#include "highgui.h"
#include <stdio.h>

// A Simple Camera Capture Framework
int main(int argc, char** argv) {
    CvCapture* capture = 0;
	IplImage* image = 0;
	CvVideoWriter* writer = 0;

    if (2 != argc)
    {
        fprintf(stderr, "Argument error\nUsage: BasicVideoTest <video_file>\n");
        return -1;
    }

	image = cvLoadImage(argv[1], 1);
    if (image)
	{
		int i = 0;
		fprintf(stderr, "Creating video from image." );
		writer = cvCreateVideoWriter("out.avi", -1, 24, cvGetSize(image), 1);
		for (i=0;i<24*3;i++)
		{
			cvWriteFrame(writer, image);
		}
		cvReleaseVideoWriter(&writer);
		fprintf(stderr, "Done." );
		return 0;
	}

    capture = cvCaptureFromFile( argv[1]);
    if( !capture )
    {
        fprintf(stderr, argv[1] );
        fprintf(stderr, "\nERROR: capture is NULL \n" );
        getchar();
        return -1;
    }
    
    printf("Camera Capture Properties:\n");
    printf("\tWidth:  %f\n", cvGetCaptureProperty(capture,
                                                  CV_CAP_PROP_FRAME_WIDTH));
    printf("\tHeight: %f\n", cvGetCaptureProperty(capture,
                                                  CV_CAP_PROP_FRAME_HEIGHT));
    printf("\tFPS:    %f\n", cvGetCaptureProperty(capture,
                                                  CV_CAP_PROP_FPS));
    // Create a window in which the captured images will be p     resented
    
    cvNamedWindow( "Raw Camera Image", CV_WINDOW_AUTOSIZE );  
    
    //     Show the image captured from the camera in the window and repeat
    while(1) {
        // Get one frame
        IplImage* frame = cvQueryFrame( capture );
        if( !frame ) 
        {
            fprintf( stderr, "ERROR: frame is null...\n" );
            getchar();
            break;
        }
        cvShowImage( "Raw Camera Image", frame );
        //Do not release the frame!
        //If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
        //remove higher bits using AND operator
        if( (cvWaitKey(10) & 255) == 27 ) break;
    }
    
    //     Release the capture device housekeeping
    cvReleaseCapture( &capture );
    cvDestroyWindow( "Raw Camera Image" );
    return 0;
}

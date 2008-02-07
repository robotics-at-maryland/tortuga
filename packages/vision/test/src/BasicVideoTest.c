
// Library Includes
#include "cv.h"
#include "highgui.h"

// STD Includes
#include <stdio.h>


// A Simple Camera Capture Framework
int main(int argc, char** argv) {
    CvCapture* capture = 0;
    IplImage* image = 0;
    IplImage* drawing = 0;
    CvVideoWriter* writer = 0;

    if (2 != argc)
    {
        fprintf(stderr, "Argument error\nUsage: BasicVideoTest <video_file>\n");
       // return -1;
    }

    image = cvLoadImage(argv[1], 1);
    drawing = cvCreateImage(cvGetSize(image), 8, 3);
    if (image)
    {
        double x, y;
        int i = 0;
        fprintf(stderr, "Creating video from image." );
        writer = cvCreateVideoWriter("out.avi", -1, 24, cvGetSize(image), 1);
        x = 50;
        y = 50;
        for (i=0;i<24*3;i++)
        {
            CvPoint p1, p2;
            cvCopyImage(image, drawing);
            p1.x = (int)x;
            p1.y = (int)y;
            p2.x = (int)x + 500;
            p2.y = (int)y+500;
            cvLine(drawing, p1, p2, CV_RGB(255, 0, 0), 10, CV_AA, 0);
            cvWriteFrame(writer, drawing);
            y += 3;
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

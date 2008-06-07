#include "cv.h"
#include "highgui.h"
#include "math.h"


void rotate90DegClockwise(IplImage* src, IplImage* dest)
{
    char* data=src->imageData;
    char* data2=dest->imageData;
    int width=src->width;
    int height=src->height;
    int height3 = height * 3;
    
    if (width!=dest->height || height!=dest->width)
    {
//        cout<<"Wrong dimensions of destination image in rotation, should be transpose of src image"<<endl;
        exit(-1);
    }
    int count=0;
    int count2=0;
    for (int y=0; y<height;y++)
    {
        count2=-3-3*y;
        for (int x=0; x<width;x++)
        {
            count2 += height3;
            data2[count2]=data[count];
            data2[count2+1]=data[count+1];
            data2[count2+2]=data[count+2];
            count+=3;
        }
    }
}    

int main( int argc, char** argv )

{

    IplImage* src;
    
/* the first command line parameter must be image file name */

    if( argc==2 && (src = cvLoadImage(argv[1], -1)) !=0 )
    {
        // An Image rotated 90 degrees
        IplImage* dst = cvCreateImage(cvSize(cvGetSize(src).height,
                                             cvGetSize(src).width), 8, 3);
        //int delta = 1;
        int angle = -90;



/*        cvNamedWindow( "src", 1 );
          cvShowImage( "src", src );*/

        for(int i = 0; i < 1000; ++i)
        {
            // Rotate with OpenCV's advanced rotation function
/*            float m[6];
            CvMat M = cvMat( 2, 3, CV_32F, m );

            double factor = -angle*CV_PI/180.0;
            m[0] = (float)(cos(factor));
            m[1] = (float)(sin(factor));
            m[2] = src->width * 0.5f;
            m[3] = -m[1];
            m[4] = m[0];
            m[5] = src->height * 0.5f;

            cvGetQuadrangleSubPix( src, dst, &M);*/

            // Rotate with our hand coded rotation functions
            rotate90DegClockwise(src, dst);
            
            // Show Results
/*            cvNamedWindow( "dst", 1 );
            cvShowImage( "dst", dst );

            if( cvWaitKey(5) == 27 )
                break;*/

        }

    }

    return 0;
}

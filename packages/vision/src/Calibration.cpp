#include "vision/include/Calibration.h"

using namespace std;
using namespace ram::vision; 

Calibration::Calibration(OpenCVCamera* camera)
{
	cam=camera;
	calibrated=false;
	frame = new ram::vision::OpenCVImage(640,480);
	cvNamedWindow("Calibration", CV_WINDOW_AUTOSIZE);
	dest=cvCreateImage(cvSize(480,640),8,3);
}

Calibration::~Calibration()
{
	delete frame;
}

void Calibration::calculateCalibrations()
{
	const int NUMIMAGES=20;

	if (calibrated)
		cout<<"Warning: This calibration is already set up.  Are you recalibrating before undistorting each image?"<<endl;
	
	CvPoint2D32f array[36*NUMIMAGES];
	int goodImages=0;
	int arrayIndex=0;
	int cornerCountsArray[NUMIMAGES];
	
	while (goodImages<NUMIMAGES)
	{
		cvWaitKey(25);
		cam->getImage(frame);
		IplImage* image =(IplImage*)(*frame);
     		rotate90Deg(image,dest);
		int cornerCount=findCorners(image,&array[arrayIndex]);
		cvShowImage("Calibration",image);

		if (cornerCount==36)
		{
			cornerCountsArray[goodImages]=36;
			arrayIndex+=36;
			goodImages++;
			cout<<goodImages<<endl;
		}
		else
		  {
		    if (cornerCount!=-1)
		      cout<<"Total failure, corner count set incorrectly"<<endl;
		  }
	}
	
	//Multiply all these by number of images
	CvPoint3D32f buffer[36*NUMIMAGES];
        for( int i = 0 ; i < NUMIMAGES ; ++i ) { 
	  for( int j = 0 ; j < 6 ; ++j ) { 
	    for( int k = 0 ; k < 6 ; ++k ) { 
	      buffer[ ( ( i * 6 + j ) * 6 + k ) ] = 
		cvPoint3D32f( j * 25.4, k * 25.4, 0 ); 
	    } 
	  } 
        } 

	//	for (int x=0; x<arrayIndex;x++)
	//{
	//	buffer[x].x=array[x].x;
	//	buffer[x].y=array[x].y;
	//	buffer[x].z=0;
	//}
	calibrateCamera(640, 480, cornerCountsArray, distortion,cameraMatrix,transVects,rotMat,NUMIMAGES,array,buffer);
	calibrated=true;
	cout<<"Calibration Complete"<<endl;
}

void Calibration::calibrateImage(IplImage* src, IplImage* dest)
{
	undistort(src, dest, cameraMatrix, distortion);
}

void Calibration::printCalibrations()
{
	int i;
	int j;
	if (!calibrated)
		calculateCalibrations();
	
	cout<<"Distortion Coefficients are:"<<endl;
	for (i=0; i<4; i++)
		cout<<distortion[i]<<endl;
		
	cout<<"The camera matrix is:"<<endl<<endl;
	for (i=0; i<3; i++)
	{
		for (j=0; j<3; j++)
			cout<<cameraMatrix[i*3+j]<<"  ";
		cout<<endl;
	}
	cout<<endl<<"The rotation vector is: "<<endl<<endl;
	for (i=0; i<3; i++)
	{
		for (j=0; j<3; j++)
		  cout<<rotMat[i*3+j]<<"  ";
		cout<<endl;
	}
	cout<<endl<<"The translation vector is: "<<endl<<endl;
	for (i=0; i<3; i++)
		cout<<transVects[i]<<"  ";
	cout<<endl;
}

void Calibration::setCalibrationManual(float* distortion2, float* cameraMatrix2, float* rotMat2, float* transVects2)
{
	if (calibrated)
		cout<<"Warning: This has already been calibrated.  Are you purposefully manually recalibrating?"<<endl;
	int i;
	for (i=0; i<4; i++)
		distortion[i]=distortion2[i];
	for (i=0; i<9; i++)
		cameraMatrix[i]=cameraMatrix2[i];
	for (i=0; i<9; i++)
		rotMat[i]=rotMat2[i];
	for (i=0; i<3;i++)
		transVects[i]=transVects2[i];
		
	calibrated=true;
}

void Calibration::setCalibrationGarbage()
{
    /*
     I hate OpenCV.  These fucking parameters aren't fucking documented, and don't
     fucking correspond to any physical model in any textbook or web site that I 
     have ever found, except this one:
     
     http://alumni.media.mit.edu/~sbeck/results/Distortion/distortion.html
     
     I've put some annotations in here to document my **guesses** about what these 
     parameters correspond to, physically.
     
     Love,
     Leo
     
     P.S. Fuck OpenCV.  Fuck you, Intel.
     */
    cout<<"Setting garbage values to see if anything happens"<<endl;
    
    // DISTORTION VALUES, presumably for exchangeable lens
    
    // Quadratic dependence, k1.  A point in the original image a distance r 
    // from the center maps k1*r^2 away from the center in the final image.
    // Units unknown.
    distortion[0]=-.4;
    
    // Quartic dependence, k2.  A point in the original image a distance r 
    // from the center maps k2*r^4 away from the center in the final image.
    // Units unknown.
    distortion[1]=-.01;
    
    // Presumably, these two parameters translate the origin of the distortion.
    // That would make these the p1 and p2 from OpenCV's documentation.
    // Possibly in pixels.
    distortion[2]=.63;
    distortion[3]=.024;
    
    
    // CAMERA PARAMETERS
    // It appears that these parameters allow the camera's "intrinsic" optics
    // to be slightly elliptical, and offset on the image.
    
    // Focal length, x-axis, units unknown
    cameraMatrix[0]=169;
    cameraMatrix[1]=0;
    // x-translation of camera's focus, relative to left of image, in pixels
    cameraMatrix[2]=342;
    cameraMatrix[3]=0;
    // Focal length, y-axis, units unknown
    cameraMatrix[4]=90;
    // y-translation of camera's focus, relative to top of image, in pixels
    cameraMatrix[5]=-158;
    cameraMatrix[6]=0;
    cameraMatrix[7]=0;
    cameraMatrix[8]=1;
    calibrated=true;
}

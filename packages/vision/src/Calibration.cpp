#include "vision/include/Calibration.h"
using namespace std;
using namespace ram::vision; 

Calibration::Calibration(OpenCVCamera* camera)
{
	cam=camera;
	calibrated=false;
	frame = new ram::vision::OpenCVImage(640,480);
	cvNamedWindow("Calibration", CV_WINDOW_AUTOSIZE);
}

Calibration::~Calibration()
{
	delete frame;
}

void Calibration::calculateCalibrations()
{
	if (calibrated)
		cout<<"Warning: This calibration is already set up.  Are you recalibrating before undistorting each image?"<<endl;
	
	CvPoint2D32f array[36*NUMIMAGES_CALIBRATE];
	int outofboundstop[20];
	CvPoint2D32f tmpArray[36];
	int outofboundsbottom[20];
	for (int test=0; test<20;test++)
		outofboundstop[test]=outofboundsbottom[test]=-27;
	for (int index=0;index<36*NUMIMAGES_CALIBRATE;index++)
	{
		CvPoint2D32f* zeroMe=&(array[index]);
		zeroMe->x=zeroMe->y=0;
	}
	int goodImages=0;
	int arrayIndex=0;
	int cornerCountsArray[NUMIMAGES_CALIBRATE];
	
	while (goodImages<NUMIMAGES_CALIBRATE)
	{
		cam->getUncalibratedImage(frame);
		IplImage* image =(IplImage*)(*frame);
		cout<<"Printing image"<<endl;
		int cornerCount=findCorners(image,tmpArray);
		for (int test=0; test<20;test++)
			if (outofboundstop[test]!=-27 || outofboundsbottom[test]!=-27)
			{
				cout<<"findCorners is fucking with memory outside the array.  I'm terminating"<<endl;
				exit(-27);
			}

		cvShowImage("Calibration",image);

		if (cornerCount==36)//This version of find corners returns -1 if the chessboard was not completely found
		{
			cout<<cornerCount<<endl;
			cout<<arrayIndex<<endl;
			cornerCountsArray[goodImages]=36;
			for (int copier=0;copier<36;copier++)
				array[arrayIndex++]=tmpArray[copier];
			goodImages++;
			cout<<goodImages<<endl;
		}
		else if (cornerCount==-1)
		{
			
		}
		else
			cout<<"ERROR!!!!"<<endl;
	}
	
	CvPoint3D32f buffer[36*NUMIMAGES_CALIBRATE];
	for( int i = 0 ; i < NUMIMAGES_CALIBRATE ; ++i ) {
		for( int j = 0 ; j < 6 ; ++j ) {
			for( int k = 0 ; k < 6 ; ++k ) {
				buffer[ ( ( i * 6 + j ) * 6 + k ) ] = cvPoint3D32f( j * 29.5, k * 29.5, 0 );
			}
		}
	}

	calibrateCamera(640, 480, cornerCountsArray, distortion,cameraMatrix,transVects,rotMat,NUMIMAGES_CALIBRATE,array,buffer);
	calibrated=true;
	cout<<"Calibration Complete"<<endl;
}

void Calibration::calibrateImage(IplImage* src, IplImage* dest)
{
	cout<<src->width<<" "<<src->height<<" "<<dest->width<<" "<<dest->height<<endl;
	undistort(src, dest, cameraMatrix, distortion);
	cout<<"Successfully undistorted"<<endl;
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
	
	FILE* handle=fopen("calibrations.out","w+");
	{
		for (i=0; i<4; i++)
			fprintf(handle, "%f ", distortion[i]);
		fprintf(handle,"\n");
		
		for (i=0; i<3; i++)
		{
			for (j=0; j<3; j++)
				fprintf(handle, "%f ",cameraMatrix[i*3+j]);
			fprintf(handle, "\n");
		}
	}
	fclose(handle);
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

void Calibration::setCalibration(bool forward)
{
	//Note that we are currently ignoring the boolean, the distortion parameters are set for the forward facing camera at the moment.
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
    
	//One good test runs values for Joe's USB camera
	//-0.109878 -1.166973 -0.001312 -0.008959
	//938.460815 0.000000 365.139343
	//0.000000 965.857849 265.445618
	//0.000000 0.000000 1.000000

	//
	//
	
	if (forward)
	{
	// DISTORTION VALUES, presumably for exchangeable lens
    
    // Quadratic dependence, k1.  A point in the original image a distance r 
    // from the center maps k1*r^2 away from the center in the final image.
    // Units unknown.
    distortion[0]=-0.109878;
    
    // Quartic dependence, k2.  A point in the original image a distance r 
    // from the center maps k2*r^4 away from the center in the final image.
    // Units unknown.
    distortion[1]=-1.166973;
    
    // Presumably, these two parameters translate the origin of the distortion.
    // That would make these the p1 and p2 from OpenCV's documentation.
    // Possibly in pixels.
    distortion[2]=-0.001312;
    distortion[3]=-0.008959;
    
    
    // CAMERA PARAMETERS
    // It appears that these parameters allow the camera's "intrinsic" optics
    // to be slightly elliptical, and offset on the image.
    
    // Focal length, x-axis, units unknown
    cameraMatrix[0]=938.460815;
    cameraMatrix[1]=0;
    // x-translation of camera's focus, relative to left of image, in pixels
    cameraMatrix[2]=365.139343;
    cameraMatrix[3]=0;
    // Focal length, y-axis, units unknown
    cameraMatrix[4]=965.857849;
    // y-translation of camera's focus, relative to top of image, in pixels
    cameraMatrix[5]=265.445618;
    cameraMatrix[6]=0;
    cameraMatrix[7]=0;
    cameraMatrix[8]=1;
	}
	else//downward
	{
	    distortion[0]=-0.109878;
    
    // Quartic dependence, k2.  A point in the original image a distance r 
    // from the center maps k2*r^4 away from the center in the final image.
    // Units unknown.
    distortion[1]=-1.166973;
    
    // Presumably, these two parameters translate the origin of the distortion.
    // That would make these the p1 and p2 from OpenCV's documentation.
    // Possibly in pixels.
    distortion[2]=-0.001312;
    distortion[3]=-0.008959;
    
    
    // CAMERA PARAMETERS
    // It appears that these parameters allow the camera's "intrinsic" optics
    // to be slightly elliptical, and offset on the image.
    
    // Focal length, x-axis, units unknown
    cameraMatrix[0]=938.460815;
    cameraMatrix[1]=0;
    // x-translation of camera's focus, relative to left of image, in pixels
    cameraMatrix[2]=365.139343;
    cameraMatrix[3]=0;
    // Focal length, y-axis, units unknown
    cameraMatrix[4]=965.857849;
    // y-translation of camera's focus, relative to top of image, in pixels
    cameraMatrix[5]=265.445618;
    cameraMatrix[6]=0;
    cameraMatrix[7]=0;
    cameraMatrix[8]=1;
	}
    calibrated=true;
}

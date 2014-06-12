/*
 * phase_correlation: 
 * load 2 images and calculate phase correlation
 *
 * Author:
 * Nashruddin Amin (http://www.nasrudin.com)
 */

#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <fftw3.h>

void phase_correlation( IplImage *ref, IplImage *tpl, IplImage *poc );

int main( int argc, char** argv )
{
	IplImage *tpl = 0;
	IplImage *ref = 0;
	IplImage *poc = 0;
	
	if( argc < 3 ) {
		fprintf( stderr, "Usage: phase_correlation <image1> <image2>\n" );
		return 1;	
	}
	
	/* load reference image */
	ref = cvLoadImage( argv[1], CV_LOAD_IMAGE_GRAYSCALE );
	
	/* always check */
	if( ref == 0 ) {
		fprintf( stderr, "Cannot load %s!\n", argv[1] );
		return 1;	
	}
	
	/* load template image */
	tpl = cvLoadImage( argv[2], CV_LOAD_IMAGE_GRAYSCALE );
	
	/* always check */
	if( tpl == 0 ) {
		fprintf( stderr, "Cannot load %s!\n", argv[2] );
		return 1;	
	}
	
	/* both images' size should be equal */
	if( ( tpl->width != ref->width ) || ( tpl->height != ref->height ) ) {
		fprintf( stderr, "Both images must have equal width and height!\n" );
		return 1;
	}
	
	/* create a new image, to store phase correlation result */
	poc = cvCreateImage( cvSize( tpl->width, tpl->height ), IPL_DEPTH_64F, 1 );
	
	/* get phase correlation of input images */
	phase_correlation( ref, tpl, poc );
	
	/* find the maximum value and its location */
    CvPoint minloc, maxloc;
	double  minval, maxval;
	cvMinMaxLoc( poc, &minval, &maxval, &minloc, &maxloc, 0 );
	
	/* print it */
	fprintf( stdout, "Maxval at (%d, %d) = %2.4f\n", maxloc.x, maxloc.y, maxval );
	cvScale(poc, poc, 1.0/(maxval-minval), 
		1.0*(-minval)/(maxval-minval));
	
	/* display images and free memory */
	cvNamedWindow( "tpl", CV_WINDOW_AUTOSIZE );
	cvNamedWindow( "ref", CV_WINDOW_AUTOSIZE );	
	cvNamedWindow( "poc", CV_WINDOW_AUTOSIZE );	
	
	cvShowImage( "tpl", tpl );
	cvShowImage( "ref", ref );
	cvShowImage( "poc", poc );
	
	cvWaitKey( 0 );
	
	cvDestroyWindow( "tpl" );
	cvDestroyWindow( "ref" );	
	cvDestroyWindow( "poc" );	
	
	cvReleaseImage( &tpl );
	cvReleaseImage( &ref );
	cvReleaseImage( &poc );
	
	return 0;
}

/*
 * get phase correlation from two images and save result to the third image
 */
void phase_correlation( IplImage *ref, IplImage *tpl, IplImage *poc )
{
	int 	i, j, k;
	double	tmp;
	
	/* get image properties */
	int width  	 = ref->width;
	int height   = ref->height;
	int step     = ref->widthStep;
	int fft_size = width * height;

	/* setup pointers to images */
	uchar 	*ref_data = ( uchar* ) ref->imageData;
	uchar 	*tpl_data = ( uchar* ) tpl->imageData;
	double 	*poc_data = ( double* )poc->imageData;
	
	/* allocate FFTW input and output arrays */
	fftw_complex *img1 = ( fftw_complex* )fftw_malloc( sizeof( fftw_complex ) * width * height );
	fftw_complex *img2 = ( fftw_complex* )fftw_malloc( sizeof( fftw_complex ) * width * height );
	fftw_complex *res  = ( fftw_complex* )fftw_malloc( sizeof( fftw_complex ) * width * height );	
	
	/* setup FFTW plans */
	fftw_plan fft_img1 = fftw_plan_dft_1d( width * height, img1, img1, FFTW_FORWARD,  FFTW_ESTIMATE );
	fftw_plan fft_img2 = fftw_plan_dft_1d( width * height, img2, img2, FFTW_FORWARD,  FFTW_ESTIMATE );
	fftw_plan ifft_res = fftw_plan_dft_1d( width * height, res,  res,  FFTW_BACKWARD, FFTW_ESTIMATE );
	
	/* load images' data to FFTW input */
	for( i = 0, k = 0 ; i < height ; i++ ) {
		for( j = 0 ; j < width ; j++, k++ ) {
			img1[k][0] = ( double )ref_data[i * step + j];
			img1[k][1] = 0.0;
			
			img2[k][0] = ( double )tpl_data[i * step + j];
			img2[k][1] = 0.0;
		}
	}
	
	/* obtain the FFT of img1 */
	fftw_execute( fft_img1 );
	
	/* obtain the FFT of img2 */
	fftw_execute( fft_img2 );
	
	/* obtain the cross power spectrum */
	for( i = 0; i < fft_size ; i++ ) {
		res[i][0] = ( img2[i][0] * img1[i][0] ) - ( img2[i][1] * ( -img1[i][1] ) );
		res[i][1] = ( img2[i][0] * ( -img1[i][1] ) ) + ( img2[i][1] * img1[i][0] );

		tmp = sqrt( pow( res[i][0], 2.0 ) + pow( res[i][1], 2.0 ) );

		res[i][0] /= tmp;
		res[i][1] /= tmp;
	}

	/* obtain the phase correlation array */
	fftw_execute(ifft_res);

	/* normalize and copy to result image */
	for( i = 0 ; i < fft_size ; i++ ) {
        poc_data[i] = res[i][0] / ( double )fft_size;
	}

	/* deallocate FFTW arrays and plans */
	fftw_destroy_plan( fft_img1 );
	fftw_destroy_plan( fft_img2 );
	fftw_destroy_plan( ifft_res );
	fftw_free( img1 );
	fftw_free( img2 );
	fftw_free( res );	
}

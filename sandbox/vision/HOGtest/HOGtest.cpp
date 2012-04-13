#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>
#include <iostream>


int main(int argc, char *argv[])
{
    //Load all images from the args
    IplImage *image = cvLoadImage(argv[1]);
    cvCvtColor(image, image, CV_BGR2YCrCb);
    
    
    uchar *dx_y = new uchar[image->height * image->width];
    uchar *dy_y = new uchar[image->height * image->width];
    uchar *dx_u = new uchar[image->height * image->width];
    uchar *dy_u = new uchar[image->height * image->width];
    uchar *dx_v = new uchar[image->height * image->width];
    uchar *dy_v = new uchar[image->height * image->width];
    
    uchar *y_data = new uchar[image->height * image->width];
    uchar *u_data = new uchar[image->height * image->width];
    uchar *v_data = new uchar[image->height * image->width];
    
    int *magnitude = new int[image->height * image->width];
    float *angle = new float[image->height * image->width];
    for(int i=0; i< image->height * image->width * 3; i+=3){
        y_data[i] = image->imageData[i];
        u_data[i] = image->imageData[i+1];
        v_data[i] = image->imageData[i+2];
    }
    
    
    for(int i=0; i< image->height; i++){
        for(int p=0; p< image->width; p++){
            //calculate dx's
            if((p == 0) || (p == (image->width-1))){
                dx_y[i*image->width+p] = 0;
                dx_u[i*image->width+p] = 0;
                dx_v[i*image->width+p] = 0;
            }
            else{
                dx_y[i*image->width+p] = y_data[i*image->width+p+1] - 
                    y_data[i*image->width+p-1];
                dx_u[i*image->width+p] = u_data[i*image->width+p+1] - 
                    u_data[i*image->width+p-1];
                dx_v[i*image->width+p] = v_data[i*image->width+p+1] - 
                    v_data[i*image->width+p-1];
            }
            
            //calculate dy's
            if((i == 0) || (i == (image->height-1))){
                dy_y[i*image->width+p] = 0;
                dy_u[i*image->width+p] = 0;
                dy_v[i*image->width+p] = 0;
            }
            else{
                dy_y[i*image->width+p] = y_data[(i+1)*image->width+p] - 
                    y_data[(i-1)*image->width+p];
                dy_u[i*image->width+p] = u_data[(i+1)*image->width+p] - 
                    u_data[(i-1)*image->width+p];
                dy_v[i*image->width+p] = v_data[(i+1)*image->width+p] - 
                    v_data[(i-1)*image->width+p];
            }
            
            //calculate angles and magnitudes per pixel
            if((i == 0) || (i == (image->height-1))
               || (p == 0) || (p == (image->width-1))){
                magnitude[i*image->width + p] = 0;
                angle[i*image->width + p] = 0;
            }
            else{
                int ymag = sqrt(dx_y[i*image->width+p] * 
                                dx_y[i*image->width+p] + 
                                dy_y[i*image->width+p] * 
                                dy_y[i*image->width+p]);
                int umag = sqrt(dx_u[i*image->width+p] * 
                                dx_u[i*image->width+p] + 
                                dy_u[i*image->width+p] * 
                                dy_u[i*image->width+p]);
                int vmag = sqrt(dx_v[i*image->width+p] * 
                                dx_v[i*image->width+p] + 
                                dy_v[i*image->width+p] * 
                                dy_v[i*image->width+p]);
                if(ymag > umag){
                    if(ymag > vmag){
                        magnitude[i*image->width+p] = ymag;
                        angle[i*image->width+p] = (!dx_y[i*image->width+p] 
                                                   ? 0 : atan(
                                                       dy_y[i*image->width+p] / 
                                                       dx_y[i*image->width+p]));
                    }
                    else{
                        magnitude[i*image->width+p] = vmag;
                        angle[i*image->width+p] = (!dx_u[i*image->width+p] 
                                                   ? 0 : atan(
                                                       dy_u[i*image->width+p] / 
                                                       dx_u[i*image->width+p]));
                    }
                }
                else if(umag>vmag){
                    magnitude[i*image->width+p] = umag;
                    angle[i*image->width+p] = (!dx_u[i*image->width+p] 
                                                   ? 0 : atan(
                                                       dy_u[i*image->width+p] / 
                                                       dx_u[i*image->width+p]));
                }
                else{
                    magnitude[i*image->width+p] = vmag;
                    angle[i*image->width+p] = (!dx_v[i*image->width+p] 
                                                   ? 0 : atan(
                                                       dy_v[i*image->width+p] / 
                                                       dx_v[i*image->width+p]));
 
                }
            }
        }
    }
    
    //We're done calculations, now lets visualize it.
    
    IplImage *out = cvCreateImage(cvSize(image->width, image->height), 
                                  IPL_DEPTH_8U, 3);
    
    //Calculate averages in a 10x10 grid
    int *avg_mag = new int[(image->height / 10) * (image->width / 10)];
    float *avg_ang = new float[(image->height / 10) * (image->width / 10)];
    for(int i=10; i<image->height; i+=10){
        for(int p=10; p<image->width; p+=10){
            if(((i+10)>image->height) || ((p+10)>image->height)){
                break;
            }
            avg_mag[(i/10) * image->width + (p/10)] = 0;
            avg_ang[(i/10) * image->width + (p/10)] = 0;
            for(int h=0; h<10; h++){
                for(int q=0; q<10; q++){
                    avg_mag[(i/10) * image->width + (p/10)] += 
                        magnitude[(i+h)*image->width + i+q]/100;
                    avg_ang[(i/10) * image->width + (p/10)] += 
                        angle[(i+h)*image->width + i+q]/100;
                }
            }
        }
    }
    
    //Draw lines on the output image
    for(int i=10; i<image->height; i+=10){
        for(int p=10; p<image->width; p+=10){
            if(((i+10)>image->height) || ((p+10)>image->height)){
                break;
            }
            int x_value = cos(avg_ang[(i/10) * image->width + (p/10)]) * 5;
            int y_value = sin(avg_ang[(i/10) * image->width + (p/10)]) * 5;
            
            // normalize it to 510 being the max (aka 255 + 255)
            int magnitude_avg = (avg_mag[(i/10) * image->width + (p/10)] * 
                                 510) / 361; 
            
            // Calculate color values for the line
            // Blue is smallest magnitude, purple is mid, red is largest
            int r_value = (magnitude_avg > 255 ? 255 : magnitude_avg);
            int b_value = (magnitude_avg > 255 ? 510 - magnitude_avg: 255);
            
            cvLine(out,                         /* the dest image */
                   cvPoint(i+5 - x_value, p+5 - y_value),             /* start point */
                   cvPoint(i+5 + x_value, p+5 + y_value),            /* end point */
                   cvScalar(b_value, 0, r_value, 0),      /* the color; green */
                   1, 8, 0);
        }
    }
    
    // show the window
    cvNamedWindow("output", CV_WINDOW_AUTOSIZE);
    cvShowImage("output", out);
    cvWaitKey(0);
    cvDestroyWindow("output");
    cvReleaseImage(&out);
    
}

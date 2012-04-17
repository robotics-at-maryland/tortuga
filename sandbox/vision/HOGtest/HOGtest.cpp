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

    int width = image->width;
    int height = image->height;
    
    
    uchar *dx_y = new uchar[height * width];
    uchar *dy_y = new uchar[height * width];
    uchar *dx_u = new uchar[height * width];
    uchar *dy_u = new uchar[height * width];
    uchar *dx_v = new uchar[height * width];
    uchar *dy_v = new uchar[height * width];
    
    uchar *y_data = new uchar[height * width];
    uchar *u_data = new uchar[height * width];
    uchar *v_data = new uchar[height * width];
    
    int *magnitude = new int[height * width];
    float *angle = new float[height * width];
    for(int i=0; i< height * width * 3; i+=3){
        y_data[i] = image->imageData[i];
        u_data[i] = image->imageData[i+1];
        v_data[i] = image->imageData[i+2];
    }
    
    
    for(int i=0; i< height; i++){
        for(int p=0; p< width; p++){
            //calculate dx's
            if((p == 0) || (p == (width-1))){
                dx_y[i*width+p] = 0;
                dx_u[i*width+p] = 0;
                dx_v[i*width+p] = 0;
            }
            else{
                dx_y[i*width+p] = y_data[i*width+p+1] - 
                    y_data[i*width+p-1];
                dx_u[i*width+p] = u_data[i*width+p+1] - 
                    u_data[i*width+p-1];
                dx_v[i*width+p] = v_data[i*width+p+1] - 
                    v_data[i*width+p-1];
            }
            
            //calculate dy's
            if((i == 0) || (i == (height-1))){
                dy_y[i*width+p] = 0;
                dy_u[i*width+p] = 0;
                dy_v[i*width+p] = 0;
            }
            else{
                dy_y[i*width+p] = y_data[(i+1)*width+p] - 
                    y_data[(i-1)*width+p];
                dy_u[i*width+p] = u_data[(i+1)*width+p] - 
                    u_data[(i-1)*width+p];
                dy_v[i*width+p] = v_data[(i+1)*width+p] - 
                    v_data[(i-1)*width+p];
            }
            
            //calculate angles and magnitudes per pixel
            if((i == 0) || (i == (height-1))
               || (p == 0) || (p == (width-1))){
                magnitude[i*width + p] = 0;
                angle[i*width + p] = 0;
            }
            else{
                int ymag = sqrt(dx_y[i*width+p] * 
                                dx_y[i*width+p] + 
                                dy_y[i*width+p] * 
                                dy_y[i*width+p]);
                int umag = sqrt(dx_u[i*width+p] * 
                                dx_u[i*width+p] + 
                                dy_u[i*width+p] * 
                                dy_u[i*width+p]);
                int vmag = sqrt(dx_v[i*width+p] * 
                                dx_v[i*width+p] + 
                                dy_v[i*width+p] * 
                                dy_v[i*width+p]);
                if(ymag > umag){
                    if(ymag > vmag){
                        magnitude[i*width+p] = ymag;
                        angle[i*width+p] = (!dx_y[i*width+p] 
                                                   ? 0 : atan(
                                                       dy_y[i*width+p] / 
                                                       dx_y[i*width+p]));
                    }
                    else{
                        magnitude[i*width+p] = vmag;
                        angle[i*width+p] = (!dx_u[i*width+p] 
                                                   ? 0 : atan(
                                                       dy_u[i*width+p] / 
                                                       dx_u[i*width+p]));
                    }
                }
                else if(umag>vmag){
                    magnitude[i*width+p] = umag;
                    angle[i*width+p] = (!dx_u[i*width+p] 
                                                   ? 0 : atan(
                                                       dy_u[i*width+p] / 
                                                       dx_u[i*width+p]));
                }
                else{
                    magnitude[i*width+p] = vmag;
                    angle[i*width+p] = (!dx_v[i*width+p] 
                                                   ? 0 : atan(
                                                       dy_v[i*width+p] / 
                                                       dx_v[i*width+p]));
 
                }
            }
        }
    }
    
    //We're done calculations, now lets visualize it.
    
    IplImage *out = cvCreateImage(cvSize(width, height), 
                                  IPL_DEPTH_8U, 3);
    
    //Calculate averages in a 10x10 grid
    int *avg_mag = new int[(height / 10) * (width / 10)];
    float *avg_ang = new float[(height / 10) * (width / 10)];
    for(int i=10; i<height; i+=10){
        for(int p=10; p<width; p+=10){
            if((i>height) || (p>width)){
                break;
            }
            avg_mag[((i-10)/10) * (width/10) + ((p-10)/10)] = 0;
            avg_ang[((i-10)/10) * (width/10) + ((p-10)/10)] = 0;
            for(int h=0; h<10; h++){
                for(int q=0; q<10; q++){
                    avg_mag[((i-10)/10) * (width/10) + ((p-10)/10)] += 
                        magnitude[(i+h)*width + (p-10)+q];
                    avg_ang[((i-10)/10) * (width/10) + ((p-10)/10)] += 
                        angle[((i-10)+h)*width + (p-10)+q];
                }
            }
            avg_mag[((i-10)/10) * (width/10) + ((p-10)/10)] = 
                avg_mag[((i-10)/10) * (width/10) + ((p-10)/10)] / 100;
            avg_ang[((i-10)/10) * (width/10) + ((p-10)/10)] = 
                avg_ang[((i-10)/10) * (width/10) + ((p-10)/10)] / 100;
        }
    }
    
    //Draw lines on the output image
    for(int i=10; i<height; i+=10){
        for(int p=10; p<width; p+=10){
            if((i>height) || (p>width)){
                break;
            }
            int x_value = cos(avg_ang[((i-10)/10) * (width/10) + 
                                      ((p-10)/10)]) * 5;
            int y_value = sin(avg_ang[((i-10)/10) * (width/10) + 
                                      ((p-10)/10)]) * 5;
            
            // normalize it to 510 being the max (aka 255 + 255)
            int magnitude_avg = (avg_mag[((i-10)/10) * (width/10) + 
                                         ((p-10)/10)] * 510) / 361; 
            
            // Calculate color values for the line
            // Blue is smallest magnitude, purple is mid, red is largest
            int r_value = (magnitude_avg > 255 ? 255 : magnitude_avg);
            int b_value = (magnitude_avg > 255 ? 510 - magnitude_avg: 255);
            
            cvLine(out,                         /* the dest image */
                   cvPoint(p+5 - x_value, i+5 - y_value),     /* start point */
                   cvPoint(p+5 + x_value, i+5 + y_value),     /* end point */
                   cvScalar(b_value, 0, r_value, 0),      /* the color; green */
                   1, 8, 0);
        }
    }
    
    // show the window
    cvNamedWindow("output", CV_WINDOW_AUTOSIZE);
    cvShowImage("output", out);
    cvNamedWindow("image", CV_WINDOW_AUTOSIZE);
    cvShowImage("image", image);
    cvWaitKey(0);
    cvDestroyWindow("output");
    cvReleaseImage(&out);
    cvDestroyWindow("image");
    cvReleaseImage(&image);
    
}

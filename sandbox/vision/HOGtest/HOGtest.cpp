#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>
#include <iostream>

//values found earlier to increase efficiency.
// 0 = tan(10 * (PI / 180))
// 1 = tan(-10 * (PI / 180))
// 2 = tan(-30 * (PI / 180))
// etc. until we reach boundary 0 again
#define BOUNDARY0 0.17632698070846498
#define BOUNDARY1 -0.17632698070846498
#define BOUNDARY2 -0.57735026918962573
#define BOUNDARY3 -1.19175359259421
#define BOUNDARY4 -2.7474774194546216
#define BOUNDARY5 -10000000000
#define BOUNDARY6 2.7474774194546225
#define BOUNDARY7 1.19175359259421
#define BOUNDARY8 0.57735026918962573
#define BOUNDARY9 0.17632698070846489
#define BOUNDARY10 -0.17632698070846489
#define BOUNDARY11 -0.57735026918962573
#define BOUNDARY12 -1.19175359259421
#define BOUNDARY13 -2.7474774194546225
#define BOUNDARY14 1000000000
#define BOUNDARY15 2.7474774194546225
#define BOUNDARY16 1.19175359259421
#define BOUNDARY17 0.57735026918962573


int main(int argc, char *argv[])
{
    //Load all images from the args
    IplImage *image = cvLoadImage(argv[1]);
    srand ( time(NULL) );

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

    uchar *dx = new uchar[height * width];
    uchar *dy = new uchar[height * width];

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

                        dx[i*width+p] = dx_y[i*width+p];
                        dy[i*width+p] = dy_y[i*width+p];
                    }
                    else{
                        magnitude[i*width+p] = vmag;
                        angle[i*width+p] = (!dx_v[i*width+p] 
                                                   ? 0 : atan(
                                                       dy_v[i*width+p] / 
                                                       dx_v[i*width+p]));

                        dx[i*width+p] = dx_v[i*width+p];
                        dy[i*width+p] = dy_v[i*width+p];
                    }
                }
                else if(umag>vmag){
                    magnitude[i*width+p] = umag;
                    angle[i*width+p] = (!dx_u[i*width+p] 
                                                   ? 0 : atan(
                                                       dy_u[i*width+p] / 
                                                       dx_u[i*width+p]));

                        dx[i*width+p] = dx_u[i*width+p];
                        dy[i*width+p] = dy_u[i*width+p];
                }
                else{
                    magnitude[i*width+p] = vmag;
                    angle[i*width+p] = (!dx_v[i*width+p] 
                                                   ? 0 : atan(
                                                       dy_v[i*width+p] / 
                                                       dx_v[i*width+p]));

                        dx[i*width+p] = dx_v[i*width+p];
                        dy[i*width+p] = dy_v[i*width+p];
                }
            }
        }
    }


    // Part 2: Partitioning pixels into bins

    unsigned char *bins = new unsigned char[height * width];
    for(int i=0; i< height; i++){
        for(int p=0; p< width; p++){
            int x = dx[i*width + p];
            int y = dy[i*width + p];
            float ratio = 0;
            if(x == 0 && y > 0){
                ratio = 100;
            }
            else if(x == 0 && y < 0){
                ratio = -100;
            }
            else if(x == 0 && y == 0){
                ratio = .0001;
            }
            else{
                ratio = y / x;
            }
            if(x>=0){
                if(y>=0){
                    //Quadrant 1
                    if(ratio < BOUNDARY0){
                        bins[i*width + p] = 0;
                    }
                    else if(ratio < BOUNDARY17){
                        bins[i*width + p] = 17;
                    }
                    else if(ratio < BOUNDARY16){
                        bins[i*width + p] = 16;
                    }
                    else if(ratio < BOUNDARY15){
                        bins[i*width + p] = 15;
                    }
                    else{
                        bins[i*width + p] = 14;
                    }
                }
                else{
                    //Quadrant 4
                    if(ratio > BOUNDARY1){
                        bins[i*width + p] = 0;
                    }
                    else if(ratio > BOUNDARY2){
                        bins[i*width + p] = 1;
                    }
                    else if(ratio > BOUNDARY3){
                        bins[i*width + p] = 2;
                    }
                    else if(ratio > BOUNDARY4){
                        bins[i*width + p] = 3;
                    }
                    else{
                        bins[i*width + p] = 4;
                    }
                }
            }
            else{
                if(y>=0){
                    //Quadrant 2
                    if(ratio > BOUNDARY10){
                        bins[i*width + p] = 9;
                    }
                    else if(ratio > BOUNDARY11){
                        bins[i*width + p] = 10;
                    }
                    else if(ratio > BOUNDARY12){
                        bins[i*width + p] = 11;
                    }
                    else if(ratio > BOUNDARY13){
                        bins[i*width + p] = 12;
                    }
                    else{
                        bins[i*width + p] = 13;
                    }
                }
                else{
                    //Quadrant 3
                    if(ratio < BOUNDARY9){
                        bins[i*width + p] = 9;
                    }
                    else if(ratio < BOUNDARY8){
                        bins[i*width + p] = 8;
                    }
                    else if(ratio < BOUNDARY7){
                        bins[i*width + p] = 7;
                    }
                    else if(ratio < BOUNDARY6){
                        bins[i*width + p] = 6;
                    }
                    else{
                        bins[i*width + p] = 5;
                    }

                }
            }
        }
    }    


    //We're done calculations, now lets visualize it.
    
    IplImage *out = cvCreateImage(cvSize(width, height), 
                                  IPL_DEPTH_8U, 3);
    
    for(int i=0; i<height; i++){
        for(int p=0; p<width; p++){
            switch(bins[i*width + p]){
            case 0:
                out->imageData[(i*width+p)*3 + 0] = 0;
                out->imageData[(i*width+p)*3 + 1] = 0;
                out->imageData[(i*width+p)*3 + 2] = 0;
                break;
            case 1:
                out->imageData[(i*width+p)*3 + 0] = 0;
                out->imageData[(i*width+p)*3 + 1] = 0;
                out->imageData[(i*width+p)*3 + 2] = 127;
                break;
            case 2:
                out->imageData[(i*width+p)*3 + 0] = 0;
                out->imageData[(i*width+p)*3 + 1] = 127;
                out->imageData[(i*width+p)*3 + 2] = 0;
                break;
            case 3:
                out->imageData[(i*width+p)*3 + 0] = 0;
                out->imageData[(i*width+p)*3 + 1] = 127;
                out->imageData[(i*width+p)*3 + 2] = 127;
                break;
            case 4:
                out->imageData[(i*width+p)*3 + 0] = 127;
                out->imageData[(i*width+p)*3 + 1] = 0;
                out->imageData[(i*width+p)*3 + 2] = 0;
                break;
            case 5:
                out->imageData[(i*width+p)*3 + 0] = 127;
                out->imageData[(i*width+p)*3 + 1] = 0;
                out->imageData[(i*width+p)*3 + 2] = 127;
                break;
            case 6:
                out->imageData[(i*width+p)*3 + 0] = 127;
                out->imageData[(i*width+p)*3 + 1] = 127;
                out->imageData[(i*width+p)*3 + 2] = 0;
                break;
            case 7:
                out->imageData[(i*width+p)*3 + 0] = 127;
                out->imageData[(i*width+p)*3 + 1] = 127;
                out->imageData[(i*width+p)*3 + 2] = 127;
                break;
            case 8:
                out->imageData[(i*width+p)*3 + 0] = 127;
                out->imageData[(i*width+p)*3 + 1] = 127;
                out->imageData[(i*width+p)*3 + 2] = 255;
                break;
            case 9:
                out->imageData[(i*width+p)*3 + 0] = 127;
                out->imageData[(i*width+p)*3 + 1] = 255;
                out->imageData[(i*width+p)*3 + 2] = 127;
                break;
            case 10:
                out->imageData[(i*width+p)*3 + 0] = 127;
                out->imageData[(i*width+p)*3 + 1] = 255;
                out->imageData[(i*width+p)*3 + 2] = 255;
                break;
            case 11:
                out->imageData[(i*width+p)*3 + 0] = 255;
                out->imageData[(i*width+p)*3 + 1] = 127;
                out->imageData[(i*width+p)*3 + 2] = 127;
                break;
            case 12:
                out->imageData[(i*width+p)*3 + 0] = 255;
                out->imageData[(i*width+p)*3 + 1] = 127;
                out->imageData[(i*width+p)*3 + 2] = 255;
                break;
            case 13:
                out->imageData[(i*width+p)*3 + 0] = 255;
                out->imageData[(i*width+p)*3 + 1] = 255;
                out->imageData[(i*width+p)*3 + 2] = 127;
                break;
            case 14:
                out->imageData[(i*width+p)*3 + 0] = 255;
                out->imageData[(i*width+p)*3 + 1] = 255;
                out->imageData[(i*width+p)*3 + 2] = 255;
                break;
            case 15:
                out->imageData[(i*width+p)*3 + 0] = 0;
                out->imageData[(i*width+p)*3 + 1] = 0;
                out->imageData[(i*width+p)*3 + 2] = 255;
                break;
            case 16:
                out->imageData[(i*width+p)*3 + 0] = 0;
                out->imageData[(i*width+p)*3 + 1] = 255;
                out->imageData[(i*width+p)*3 + 2] = 0;
                break;
            case 17:
                out->imageData[(i*width+p)*3 + 0] = 255;
                out->imageData[(i*width+p)*3 + 1] = 0;
                out->imageData[(i*width+p)*3 + 2] = 0;
                break;
            case 18:
                out->imageData[(i*width+p)*3 + 0] = 255;
                out->imageData[(i*width+p)*3 + 1] = 0;
                out->imageData[(i*width+p)*3 + 2] = 255;
                break;
            }
        }
    }
    // show the window
    cvNamedWindow("output", CV_WINDOW_AUTOSIZE);
    cvShowImage("output", out);
    //cvNamedWindow("image", CV_WINDOW_AUTOSIZE);
    //cvShowImage("image", image);
    cvWaitKey(0);
    cvDestroyWindow("output");
    cvReleaseImage(&out);
    //cvDestroyWindow("image");
    //cvReleaseImage(&image);
    
}

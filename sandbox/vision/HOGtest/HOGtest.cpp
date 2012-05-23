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
    //Load the image from the args
    cv::Mat image = cv::imread(argv[1]);

    int width = image.cols;
    int height = image.rows;
    
    cv::Mat dx_channels(height, width, CV_16SC3);
    cv::Mat dy_channels(height, width, CV_16SC3);
    
    cv::Mat dx(height, width, CV_16SC2);

    cv::Mat channel_mag(height, width, CV_32FC3);
    cv::Mat channel_ang(height, width, CV_32FC3);

    cv::Mat mag_ang(height, width, CV_32FC2);

    for(int row = 0; row < image.rows; row++)
    {
        // get a pointer to the first pixel in the current row
        unsigned char* rowPtr = image.ptr<unsigned char>(row);
        short* dxRowPtr = dx_channels.ptr<short>(row);
        short* dyRowPtr = dy_channels.ptr<short>(row);

        unsigned char* rowPtrM = NULL;
        unsigned char* rowPtrP = NULL;
        if(row != 0){
            rowPtrM = image.ptr<unsigned char>(row-1);
        }
        if(row != image.rows-1){
            rowPtrP = image.ptr<unsigned char>(row+1);
        }

        float* magRowPtr = channel_mag.ptr<float>(row);
        float* angRowPtr = channel_ang.ptr<float>(row);

        float* mag_angRowPtr = mag_ang.ptr<float>(row);

        short* derivRowPtr = dx.ptr<short>(row);
        
        
        for(int col = 0; col < image.cols; col++)
        {
            float maxMag = 0;
            float maxAng = 0;
            int maxDx = 0;
            int maxDy = 0;
            for(int ch = 0; ch < image.channels(); ch++)
            {
                // calculate dx's per pixel and channel
                if((col == 0) || (col == image.cols-1)){
                    dxRowPtr[col * image.channels() + ch] = 0;
                }
                else{
                    dxRowPtr[col * image.channels() + ch] = 
                        rowPtr[(col + 1) * image.channels() + ch] - 
                        rowPtr[(col - 1) * image.channels() + ch];
                }

                // calculate dy's per pixel and channel
                if((row == 0) || (row == image.rows-1)){
                    dyRowPtr[col * image.channels() + ch] = 0;
                }
                else{
                    dyRowPtr[col * image.channels() + ch] = 
                        rowPtrP[(col) * image.channels() + ch] - 
                        rowPtrM[(col) * image.channels() + ch];

                }

                // calculate magnitudes and angles
                if((col == 0) || (col == image.cols-1) ||
                   (row == 0) || (row == image.rows-1)){
                    magRowPtr[col * image.channels() + ch] = 0;
                    angRowPtr[col * image.channels() + ch] = 0;
                }
                else{
                    magRowPtr[col * image.channels() + ch] =
                        sqrt(dxRowPtr[col * image.channels() + ch] * 
                             dxRowPtr[col * image.channels() + ch] + 
                             dyRowPtr[col * image.channels() + ch] * 
                             dyRowPtr[col * image.channels() + ch]);

                    angRowPtr[col * image.channels() + ch] =
                        (!dxRowPtr[col * image.channels() + ch] ? 0 :
                         atan(dyRowPtr[col * image.channels() + ch] /
                              dxRowPtr[col * image.channels() + ch])
                            );
                }
                if(magRowPtr[col * image.channels() + ch] > maxMag){
                    maxMag = magRowPtr[col * image.channels() + ch];
                    maxAng = angRowPtr[col * image.channels() + ch];
                    maxDx = dxRowPtr[col * image.channels() + ch];
                    maxDy = dyRowPtr[col * image.channels() + ch];
                }
            }

            //insert max magnitude, angle, and derivatives
            mag_angRowPtr[col * 2] = maxMag;
            mag_angRowPtr[col * 2 + 1] = maxAng;

            derivRowPtr[col * 2] = maxDx;
            derivRowPtr[col * 2 + 1] = maxDy;
        }
    }
    

    // Part 2: Partitioning pixels into bins

    cv::Mat bins(height, width, CV_8UC1);

    for(int row = 0; row < image.rows; row++)
    {
        unsigned char* rowPtr = bins.ptr<unsigned char>(row);
        
        short* dxRowPtr = dx.ptr<short>(row);

        for(int col = 0; col < image.cols; col++)
        {
            int x = dxRowPtr[col * 2];
            int y = dxRowPtr[col * 2 + 1];
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
                        rowPtr[col] = 0;
                    }
                    else if(ratio < BOUNDARY17){
                        rowPtr[col] = 17;
                    }
                    else if(ratio < BOUNDARY16){
                        rowPtr[col] = 16;
                    }
                    else if(ratio < BOUNDARY15){
                        rowPtr[col] = 15;
                    }
                    else{
                        rowPtr[col] = 14;
                    }
                }
                else{
                    //Quadrant 4
                    if(ratio > BOUNDARY1){
                        rowPtr[col] = 0;
                    }
                    else if(ratio > BOUNDARY2){
                        rowPtr[col] = 1;
                    }
                    else if(ratio > BOUNDARY3){
                        rowPtr[col] = 2;
                    }
                    else if(ratio > BOUNDARY4){
                        rowPtr[col] = 3;
                    }
                    else{
                        rowPtr[col] = 4;
                    }
                }
            }
            else{
                if(y>=0){
                    //Quadrant 2
                    if(ratio > BOUNDARY10){
                        rowPtr[col] = 9;
                    }
                    else if(ratio > BOUNDARY11){
                        rowPtr[col] = 10;
                    }
                    else if(ratio > BOUNDARY12){
                        rowPtr[col] = 11;
                    }
                    else if(ratio > BOUNDARY13){
                        rowPtr[col] = 12;
                    }
                    else{
                        rowPtr[col] = 13;
                    }
                }
                else{
                    //Quadrant 3
                    if(ratio < BOUNDARY9){
                        rowPtr[col] = 9;
                    }
                    else if(ratio < BOUNDARY8){
                        rowPtr[col] = 8;
                    }
                    else if(ratio < BOUNDARY7){
                        rowPtr[col] = 7;
                    }
                    else if(ratio < BOUNDARY6){
                        rowPtr[col] = 6;
                    }
                    else{
                        rowPtr[col] = 5;
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
            switch(bins.at<unsigned char>(i,p)){
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

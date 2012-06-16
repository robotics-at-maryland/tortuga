#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <algorithm>

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

#define THRESHOLD 0.2

cv::Mat HOGFeatures(cv::Mat &image){

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
            float x = dxRowPtr[col * 2];
            float y = dxRowPtr[col * 2 + 1];
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
                        rowPtr[col] = 1;
                    }
                    else if(ratio < BOUNDARY16){
                        rowPtr[col] = 2;
                    }
                    else if(ratio < BOUNDARY15){
                        rowPtr[col] = 3;
                    }
                    else{
                        rowPtr[col] = 4;
                    }
                }
                else{
                    //Quadrant 4
                    if(ratio > BOUNDARY1){
                        rowPtr[col] = 0;
                    }
                    else if(ratio > BOUNDARY2){
                        rowPtr[col] = 17;
                    }
                    else if(ratio > BOUNDARY3){
                        rowPtr[col] = 16;
                    }
                    else if(ratio > BOUNDARY4){
                        rowPtr[col] = 15;
                    }
                    else{
                        rowPtr[col] = 14;
                    }
                }
            }
            else{
                if(y>=0){
                    //Quadrant 2
                    if(ratio > BOUNDARY10){
                        rowPtr[col] = 5;
                    }
                    else if(ratio > BOUNDARY11){
                        rowPtr[col] = 6;
                    }
                    else if(ratio > BOUNDARY12){
                        rowPtr[col] = 7;
                    }
                    else if(ratio > BOUNDARY13){
                        rowPtr[col] = 8;
                    }
                    else{
                        rowPtr[col] = 9;
                    }
                }
                else{
                    //Quadrant 3
                    if(ratio < BOUNDARY9){
                        rowPtr[col] = 10;
                    }
                    else if(ratio < BOUNDARY8){
                        rowPtr[col] = 11;
                    }
                    else if(ratio < BOUNDARY7){
                        rowPtr[col] = 12;
                    }
                    else if(ratio < BOUNDARY6){
                        rowPtr[col] = 13;
                    }
                    else{
                        rowPtr[col] = 14;
                    }

                }
            }
        }
    }

    
    // Part 3: Spatial Aggregation

    
    int bin_size = 8;
    
    float cellWidthF = width / ((float) bin_size);
    float cellHeightF = height / ((float) bin_size);

    int cellWidth = (int) std::floor(cellWidthF + 0.5);
    int cellHeight = (int) std::floor(cellHeightF + 0.5);

    cv::Mat cells = cv::Mat(cellHeight, cellWidth, cv::DataType< 
                            cv::Vec<double, 32> >::type);

    for(int row = 0; row < image.rows; row++) {
        unsigned char* binPtr = bins.ptr<unsigned char>(row);
        
        float* magPtr = mag_ang.ptr<float>(row);

        for(int col = 0; col < image.cols; col++){
    
            // calculate the location of where the pixel falls in cell space
            double xCellCoord = (col + 0.5) / bin_size - 0.5;
            double yCellCoord = (row + 0.5) / bin_size - 0.5;
            
            // calculate the index of the lower index cell (closest to (0,0)) that the pixel contributes to
            int xCellIdx = (int) std::floor(xCellCoord);
            int yCellIdx = (int) std::floor(yCellCoord);

            if(xCellIdx < 0){
                xCellIdx = 0;
            }
            if(yCellIdx < 0){
                yCellIdx = 0;
            }
            
            
            // calculate the fractions that this pixel contributes to each cell
            double fracLowerX = xCellCoord - xCellIdx;
            double fracLowerY = yCellCoord - yCellIdx;
            double fracUpperX = 1.0 - fracLowerX;
            double fracUpperY = 1.0 - fracLowerY;
            
            float magnitude = magPtr[col * 2];

            int orientationBin = binPtr[col];

            // add the contributions of this pixel to the lower cell
            if((xCellIdx  <= cellHeight) && (yCellIdx <= cellWidth)){
                cells.at< cv::Vec<double, 32> >(yCellIdx + 0, 
                                                xCellIdx + 0)[orientationBin] +=
                    fracLowerX * fracLowerY * magnitude;
            }
                
            if((xCellIdx <= cellHeight) && (yCellIdx + 1 <= cellWidth)){
                cells.at< cv::Vec<double, 32> >(yCellIdx + 1, 
                                                xCellIdx + 0)[orientationBin] 
                    += fracUpperX * fracLowerY * magnitude;
            }
            if((xCellIdx + 1 <= cellHeight) && (yCellIdx <= cellWidth)){
                cells.at< cv::Vec<double, 32> >(yCellIdx + 0, 
                                                xCellIdx + 1)[orientationBin] 
                    += fracLowerX * fracUpperY * magnitude;
            }
            if((xCellIdx + 1 <= cellHeight) && (yCellIdx + 1 <= cellWidth)){
                cells.at< cv::Vec<double, 32> >(yCellIdx + 1, 
                                                xCellIdx + 1)[orientationBin] 
                    += fracUpperX * fracUpperY * magnitude;
            }
            
        }
    }
    
    
    // Part 4

    cv::Mat cellEnergy(cellHeight, cellWidth, CV_32SC1);
    
    for(int row = 0; row < cellHeight; row++) {
        
        int* cellEnergyPtr = cellEnergy.ptr<int>(row);
        
        for(int col = 0; col < cellWidth; col++) {
            
            cv::Vec<double, 32> &thisCell = 
                cells.at< cv::Vec<double, 32> >(row, col);

            // Compute undirected magnitudes
            thisCell[18] = thisCell[0] + thisCell[9];
            thisCell[19] = thisCell[1] + thisCell[10];
            thisCell[20] = thisCell[2] + thisCell[11];
            thisCell[21] = thisCell[3] + thisCell[12];
            thisCell[22] = thisCell[4] + thisCell[13];
            thisCell[23] = thisCell[5] + thisCell[14];
            thisCell[24] = thisCell[6] + thisCell[15];
            thisCell[25] = thisCell[7] + thisCell[16];
            thisCell[26] = thisCell[8] + thisCell[17];
            thisCell[27] = 0;
            thisCell[28] = 0;
            thisCell[29] = 0;
            thisCell[30] = 0;
            thisCell[31] = 0;

            // Compute energy of each cell
            cellEnergyPtr[col] = thisCell[18] * thisCell[18] + 
                thisCell[19] * thisCell[19] + thisCell[20] * thisCell[20] +
                thisCell[21] * thisCell[21] + thisCell[22] * thisCell[22] +
                thisCell[23] * thisCell[23] + thisCell[24] * thisCell[24] +
                thisCell[25] * thisCell[25] + thisCell[26] * thisCell[26];
                
            
        }
    }    

    cv::Mat blockEnergy(cellHeight-1, cellWidth-1, CV_32SC1);

    for(int row = 0; row < cellHeight-1; row++) {
        
        int* cellEnergyPtr = cellEnergy.ptr<int>(row);
        int* cellEnergyP1Ptr = cellEnergy.ptr<int>(row+1);

        int* blockEnergyPtr = blockEnergy.ptr<int>(row);
        
        for(int col = 0; col < cellWidth-1; col++) {

            // Compute block energy
            blockEnergyPtr[col] = cellEnergyPtr[col] + cellEnergyPtr[col+1] +
                cellEnergyP1Ptr[col] + cellEnergyP1Ptr[col+1];
        }
    }

    //Compute a normalized feature vector
    // cv::Mat featureVector = cv::Mat(cellHeight, cellWidth, cv::DataType< 
    //                      cv::Vec<double, 32> >::type);

    for(int row = 0; row < cellHeight; row++) {
       
        if(row != 0 && row != cellHeight-1){
            
            int* blockEnergyPtr = blockEnergy.ptr<int>(row);
            int* blockEnergyM1Ptr = blockEnergy.ptr<int>(row-1);

            
            for(int col = 0; col < cellWidth; col++) {
                if(col != 0 && col != cellWidth-1){
                    int norm3 = blockEnergyM1Ptr[col - 1];
                    int norm1 = blockEnergyM1Ptr[col];
                    int norm2 = blockEnergyPtr[col - 1];
                    int norm0 = blockEnergyPtr[col];

                    cv::Vec<double, 32> &thisCell = 
                        cells.at< cv::Vec<double, 32> >(row, col);
                    for(int i=0; i<18; i++){
                        double oldValue = thisCell[i];
                        thisCell[i] = (1/2) * 
                            (std::min(oldValue * norm0, THRESHOLD) +
                             std::min(oldValue * norm1, THRESHOLD) +
                             std::min(oldValue * norm2, THRESHOLD) +
                             std::min(oldValue * norm3, THRESHOLD));
                        
                        thisCell[28] += std::min(oldValue * norm0, THRESHOLD);
                        thisCell[29] += std::min(oldValue * norm1, THRESHOLD);
                        thisCell[30] += std::min(oldValue * norm2, THRESHOLD);
                        thisCell[31] += std::min(oldValue * norm3, THRESHOLD);
                    }
                    for(int i=18; i<27; i++){
                        double oldValue = thisCell[i];
                        thisCell[i] = (1/2) * 
                            (std::min(oldValue * norm0, THRESHOLD) +
                             std::min(oldValue * norm1, THRESHOLD) +
                             std::min(oldValue * norm2, THRESHOLD) +
                             std::min(oldValue * norm3, THRESHOLD));
                        
                    }
                    for(int i=27; i<=30; i++){
                        // yes this is a magic number, 
                        // no i don't know why it is important
                        thisCell[i] *= 0.2357;
                    }
                }
            }
        }
    }
    
    return cv::Mat(cells, cv::Rect(1, 1, cells.cols - 2, 
                                   cells.rows - 2));
}

int main(int argc, char *argv[])
{
    //Load the image from the args
    cv::Mat image = cv::imread(argv[1]);
    image = HOGFeatures(image);

    cv::Mat myTemplate = cv::imread(argv[2]);
    myTemplate = HOGFeatures(myTemplate);

    cv::Mat output;

    cv::matchTemplate(image, myTemplate, output, CV_TM_CCORR);


    output = output * 255;

    // show the window
    cv::imshow("output", output);
    cv::waitKey(0);
    
}

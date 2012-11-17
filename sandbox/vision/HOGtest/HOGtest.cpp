/* Copyright 2012 Gary Sullivan
 * Copyright 2012 Robotics @ Maryland
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <algorithm>

#include <time.h>

#include "LibSVMsetup.h"

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


int binning(float dx, float dy){

	//float angle = ( !dx ? 0 : atan((float)dy / dx) );

    float ratio = 0;
    if(dx == 0 && dy > 0){
		ratio = 100;
	}
	else if(dx == 0 && dy < 0){
		ratio = -100;
	}
	else if(dx == 0 && dy == 0){
		ratio = .0001;
	}
	else{
		ratio = dy / dx;
	}
	if(dx>=0){
		if(dy>=0){
			//Quadrant 1
			if(ratio < BOUNDARY0){
				return 0;
			}
			else if(ratio < BOUNDARY17){
				return 1;
			}
			else if(ratio < BOUNDARY16){
				return 2;
			}
			else if(ratio < BOUNDARY15){
				return 3;
			}
			else{
				return 4;
			}
		}
		else{
			//Quadrant 4
			if(ratio > BOUNDARY1){
				return 0;
			}
			else if(ratio > BOUNDARY2){
				return 17;
			}
			else if(ratio > BOUNDARY3){
				return 16;
			}
			else if(ratio > BOUNDARY4){
				return 15;
			}
			else{
				return 14;
			}
		}
	}
	else{
		if(dy>=0){
			//Quadrant 2
			if(ratio > BOUNDARY10){
				return 9;
			}
			else if(ratio > BOUNDARY11){
				return 8;
			}
			else if(ratio > BOUNDARY12){
				return 7;
			}
			else if(ratio > BOUNDARY13){
				return 6;
			}
			else{
				return 5;
			}
		}
		else{
			//Quadrant 3
			if(ratio < BOUNDARY9){
				return 10;
			}
			else if(ratio < BOUNDARY8){
				return 11;
			}
			else if(ratio < BOUNDARY7){
				return 12;
			}
			else if(ratio < BOUNDARY6){
				return 13;
			}
			else{
				return 14;
			}
			
		}
	} 
}	

cv::Mat HOGFeatures(cv::Mat &image){
    
    int width = image.cols;
    int height = image.rows;
    
    cv::Mat dx(height, width, CV_16SC2);
    
    cv::Mat mag_ang(height, width, CV_32FC2);
    
    cv::Mat bins(height, width, CV_8UC1);
	
	for(int row = 0; row < image.rows; row++)
	{
		// get a pointer to the first pixel in the current row
		unsigned char* rowPtr = image.ptr<unsigned char>(row);
		
		unsigned char* rowPtrM = NULL;
		unsigned char* rowPtrP = NULL;
		if(row != 0){
			rowPtrM = image.ptr<unsigned char>(row-1);
		}
        if(row != image.rows-1){
			rowPtrP = image.ptr<unsigned char>(row+1);   
        }            

		float* mag_angRowPtr = mag_ang.ptr<float>(row);
		
        short* derivRowPtr = dx.ptr<short>(row);
		
		
		unsigned char* binPtr = bins.ptr<unsigned char>(row);
		
		
		for(int col = 0; col < image.cols; col++)
		{
			float maxMag = 0;
			float maxAng = 0;
			int maxDx = 0;
			int maxDy = 0;
			
			int *xDerivs = new int[image.channels()];
			int *yDerivs = new int[image.channels()];

			float *magChannel = new float[image.channels()];
			float *angChannel = new float[image.channels()];
				
            for(int ch = 0; ch < image.channels(); ch++)
            {
                // calculate dx's per pixel and channel
                if((col == 0) || (col == image.cols-1)){
                   xDerivs[ch] = 0;
                }
                else{
                    xDerivs[ch] = rowPtr[(col + 1) * image.channels() + ch] - 
                        rowPtr[(col - 1) * image.channels() + ch];
                }

                // calculate dy's per pixel and channel
                if((row == 0) || (row == image.rows-1)){
                    yDerivs[ch] = 0;
                }
                else{
                    yDerivs[ch] = rowPtrP[(col) * image.channels() + ch] - 
                        rowPtrM[(col) * image.channels() + ch];

                }

                // calculate magnitudes and angles
                if((col == 0) || (col == image.cols-1) ||
                   (row == 0) || (row == image.rows-1)){
                    magChannel[ch] = 0;
                    angChannel[ch] = 0;
                }
                else{
                    magChannel[ch] =
                        sqrt((float) xDerivs[ch] * 
                             xDerivs[ch] + 
                             yDerivs[ch] * 
                             yDerivs[ch]);

                    angChannel[ch] =
                        (!xDerivs[ch] ? 0 : atan((float) yDerivs[ch] / xDerivs[ch]));
                }
                if(magChannel[ch] > maxMag){
                    maxMag = magChannel[ch];
                    maxAng = angChannel[ch];
                    maxDx = xDerivs[ch];
                    maxDy = yDerivs[ch];
                }


            }

            //insert max magnitude, angle, and derivatives
            mag_angRowPtr[col * 2] = maxMag;
            mag_angRowPtr[col * 2 + 1] = maxAng;

            derivRowPtr[col * 2] = maxDx;
            derivRowPtr[col * 2 + 1] = maxDy;

			// Part 2: Partitioning pixels into bins
			binPtr[col] = binning(maxDx, maxDy);

			delete [] xDerivs;
			delete [] yDerivs;
			delete [] magChannel;
			delete [] angChannel;
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
            if((xCellIdx  < cellWidth) && (yCellIdx < cellHeight)){
				cv::Vec<double, 32> *cellPtr = cells.ptr< cv::Vec<double, 32> >(yCellIdx); 

				cellPtr[xCellIdx][orientationBin] +=
                    fracLowerX * fracLowerY * magnitude;
            }
                
            if((xCellIdx < cellWidth) && (yCellIdx + 1 < cellHeight)){
				cv::Vec<double, 32> *cellPtr = cells.ptr< cv::Vec<double, 32> >(yCellIdx+1);

				cellPtr[xCellIdx][orientationBin] += 
					fracUpperY * fracLowerX * magnitude;
            }
            if((xCellIdx + 1 < cellWidth) && (yCellIdx < cellHeight)){
				cv::Vec<double, 32> *cellPtr = cells.ptr< cv::Vec<double, 32> >(yCellIdx);

				cellPtr[xCellIdx + 1][orientationBin] += 
					fracLowerY * fracUpperX * magnitude;
            }
            if((xCellIdx + 1 < cellWidth) && (yCellIdx + 1 < cellHeight)){
				cv::Vec<double, 32> *cellPtr = cells.ptr< cv::Vec<double, 32> >(yCellIdx+1);

				cellPtr[xCellIdx+1][orientationBin] += 
					fracUpperX * fracUpperY * magnitude;
            }
            
        }
    }
    
    
    // Part 4

    cv::Mat cellEnergy(cellHeight, cellWidth, CV_32SC1);
    
    for(int row = 0; row < cellHeight; row++) {
        
        int* cellEnergyPtr = cellEnergy.ptr<int>(row);

		cv::Vec<double, 32> *cellPtr = cells.ptr< cv::Vec<double, 32> >(row);
        
        for(int col = 0; col < cellWidth; col++) {
            
            cv::Vec<double, 32> &thisCell = cellPtr[col];

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
            
			cv::Vec<double, 32> *cellPtr = cells.ptr< cv::Vec<double, 32> >(row);
            
            for(int col = 0; col < cellWidth; col++) {
                if(col != 0 && col != cellWidth-1){
                    int norm3 = blockEnergyM1Ptr[col - 1];
                    int norm1 = blockEnergyM1Ptr[col];
                    int norm2 = blockEnergyPtr[col - 1];
                    int norm0 = blockEnergyPtr[col];
                    
                    cv::Vec<double, 32> &thisCell = cellPtr[col];
                    
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
    
    cv::Mat ret =  cv::Mat(cells, cv::Rect(1, 1, cells.cols - 2, 
                                   cells.rows - 2));

    ret.convertTo(ret, CV_32FC(32));

    return ret;
}

int main(int argc, char *argv[])
{
    runSetup();
	// clock_t start, end;

	// start = clock();
    // //Load the image from the args
    // cv::Mat image = cv::imread(argv[1]);
    // HOGFeatures(image).convertTo(image, CV_32FC(32));

    // cv::Mat myTemplate = cv::imread(argv[2]);
	// HOGFeatures(myTemplate).convertTo(myTemplate, CV_32FC(32));

    // cv::Mat output;

    // cv::matchTemplate(image, myTemplate, output, CV_TM_CCORR);
	 

	// float minNum = 1000000000;
	// float maxNum = -1000000000;

	// for(int row = 0; row < output.rows; row++)
    // {
    //     float* rowPtr = output.ptr<float>(row);

    //     for(int col = 0; col < output.cols; col++)
    //     {
    //         if(rowPtr[col] < minNum){
    //             minNum = rowPtr[col];
    //         }
    //         if(rowPtr[col] > maxNum){
    //             maxNum = rowPtr[col];
    //         }
    //     }
    // }

    // output -= minNum;
    // output *= (1 / (maxNum - minNum));

    // resize(output, output, cv::Size(), 8, 8);
	
	// end = clock();

	// std::cout << "Time required for execution: " << (double)(end-start)/CLOCKS_PER_SEC << " seconds." << std::endl;

    // // show the window
    // cv::imshow("output", output);
    // cv::waitKey(0);

    // CV_64FC(32) == cv::DataType<cv::Vec<double, 32> >::type
    // CV_32FC(32) == cv::DataType<cv::Vec<float, 32> >::type
}

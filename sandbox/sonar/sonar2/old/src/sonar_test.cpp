//Testing the operation of the sonar

#define FRAME_SIZE 2048 //needs to be divisible by 2!
#define NUM_PINGERS 4

#include <stdio.h>
#include <unistd.h>
#include "/home/levashom/auv/sonar/sonarController/include/Sonar.h"
#include "time_diff.h"

using namespace ram::sonar;
using namespace std;

int main(int argc, char* const argv[])
{
	int index;
	//Initialize the array to store incoming data
	int *raw_data[NUM_PINGERS];
	for(int k=0; k<NUM_PINGERS;k++)
	{
		raw_data[k]=new int[FRAME_SIZE*2-1];
		for(int j=0; j<FRAME_SIZE; j++)
			raw_data[k][j]=0;
	}

	FILE *f;
	adcdata_t sample[NUM_PINGERS];
	time_diff* mytdiff=new time_diff(NUM_PINGERS, FRAME_SIZE, (int)30e4, (int)32e3, (int)28e3);
	double tdiff;

	if (argc >= 2)
	{
		f = fopen(argv[1], "rb");
		if (f == NULL)
		{
			fprintf(stderr,"Could not open file.");
			exit(-1);
		}
	}
	else
	{
		fprintf(stderr,"No file specified.\n");
		exit(-1);
	}
	
	//Start the main loop
	for (index=FRAME_SIZE; true; index++)
	{
		if(index>=FRAME_SIZE*2)
			index-=FRAME_SIZE;

		//First read in the data, copy it over into our array
		fread(sample, sizeof(adcdata_t), NUM_PINGERS, f);
		if(feof(f)) break;
		for(int k=0;k<NUM_PINGERS;k++)
		{
			raw_data[k][index]=sample[k];
			raw_data[k][index-FRAME_SIZE]=sample[k];
		}

		//Tiledfft on the current frame (Leo's function)
		//
		//store the fft stuff in the frequency frame 
	
		//does the frequency frame meet a certain condition?
		//(Such as threshold level or an intergral over a region condition?)
		if(true)//!meet_condition)
		{
			if(mytdiff->update(raw_data)!=0)
			{
				printf("Error updating the data sample\n");
				exit(1);
			}
			for(int k=0; k<NUM_PINGERS-1;k++)
				for(int j=k+1;j<NUM_PINGERS;j++)
					if(mytdiff->calc_time_diff(k,j,tdiff)!=0)
					{
						printf("Error calculating tdiff!\n");
						exit(-1);
					}
					else
					{
						if(tdiff>FRAME_SIZE/2)
							tdiff-=FRAME_SIZE;
						printf("Between %d and %d: %f points\n",k+1,j+1,tdiff);
					}
			//send data to the Mac Mini

			index=FRAME_SIZE;
			//Clear the data from the array
			for(int k=0; k<NUM_PINGERS;k++)
				for(int j=0; j<FRAME_SIZE; j++)
					raw_data[k][j]=0;
		}
	} //main for loop
	fclose(f);
	delete mytdiff;

	return 0;
}
		//IN THE MINI: analyze the time difference data to extract positions -> incorporate a Kalman filter with IMU data?

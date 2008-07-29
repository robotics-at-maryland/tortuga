/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Michael Levashov
 * All rights reserved.
 *
 * Author: Michael Levashov
 * File:  packages/sonar/src/sonard/main.cpp
 */

/* 
 * This is the main program to run the sonar.
 * It starts off by acquiring a large dataset.
 * It then runs a detection algorithm on it, which gives it the direction from the pinger and the point in the data set at which the ping was found (for hydrophone 0)
 * It passes these parameters onto the main computer for use with the AI 
 * The program times itself from the start of the data collection till the algorithm has completed.
 * From the measurements, it can approximate when the next ping comes, so that it needs to take a smaller dataset
 *
 * The program loops infinitely until the blackfin is shut down or it receives a terminate signal (???) from the main computer
 */

// STD includes
#include <unistd.h>
#include <iostream>

// UNIX Includes
#include <sys/time.h>

// Project Includes
//#include "math/include/MatrixN.h"
//#include "math/include/Vector3.h"

#include "sonar/include/SonarPing.h"
#include "sonar/include/GetDirEdge.h"
#include "sonar/include/Sonar.h"

#include "drivers/bfin_spartan/include/dataset.h"
#include "drivers/bfin_spartan/include/report.h"
#include "drivers/bfin_spartan/include/spartan.h"

/*#include "SparseSDFTSpectrum.h"
#include "pingDetect.h"
#include "getPingChunk.h"*/

using namespace ram::sonar;
using namespace ram::math;
using namespace std;

void sleepTillPing(struct timeval *start_time, int point_num);
dataset* getDataset(dataset *dataSet, int length);

int main(int argc, char* argv[])
{
    struct dataset * dataSet = NULL;
    sonarPing ping;
    int ping_found;
    int do_loop=0; //whether to continue the main loop or not
    int status=-1;   //FOUR states: 0-couldn't find with large dataset,  1-couldn't find with small, 2-found with large, 3-found with small
    int loop_counter=0;
    struct timeval start_time;
    struct timeval curr_time;
    struct timeval temp_time;
    int sleep_time;

    //Initialize the communication port to the main cpu
    int fd=openDevice();

    //Get the starting time
    cout<<"Starting\n";
    gettimeofday(&start_time, NULL);
            //cout<<"Now: "<<start_time.tv_sec<<" "<<start_time.tv_usec<<endl;
    
    int myKBands[nKBands];
    for (int i = 0 ; i < nKBands ; i ++)
        myKBands[i] = kBands[i];
    for (int argIndex = 1 ; argIndex < argc ; argIndex ++)
    {
        if (strcmp(argv[argIndex], "--swap-bands") == 0)
        {
            cout << "Swapping bands" << endl;
            int temp = myKBands[0];
            myKBands[0] = myKBands[1];
            myKBands[1] = temp;
        } else {
            cout<<"Using dataset \n"<<argv[argIndex]<<endl;
            dataSet = loadDataset(argv[argIndex]);
            do_loop=0;
        }
    }
    if(dataSet == NULL)
    {
        dataSet=getDataset(dataSet, status);
        do_loop=1; //infinite loop, since I am running off the hydrophones
    }
    getDirEdge edge_detector(myKBands);
    
    do
    {
        if(loop_counter!=0) //already loaded the dataset for the first run
        {
            gettimeofday(&start_time, NULL);
            //cout<<"Now: "<<start_time.tv_sec<<" "<<start_time.tv_usec<<endl;
            dataSet=getDataset(dataSet, status);
        }

        if(dataSet == NULL)
        {
            cout<<"Could not load dataset!\n"; //but maybe I'll get luckier the next time?
            status=0;
        }
        else
        {
            if((ping_found=edge_detector.getEdge(&ping, dataSet))==0)
            {
                cout<<"No ping found\n";
                if(status>1) status=1;
                else status=0;
            }
            else if(ping_found!=1)
            {
                cout<<"Error finding ping \n"<<ping_found<<endl;
                if(status>1) status=1;
                else status=0;
            }
            else 
            {
                if(status<1) status+=2; //jump two states at once
                else if(status<3) status++;

                //Find how far into the time sample the ping was found
                temp_time.tv_sec=ping.point_num/SAMPRATE;
                temp_time.tv_usec=(((ping.point_num-(SAMPRATE*temp_time.tv_sec))*1000)/SAMPRATE)*1000;

                //Figure out the time at which the ping happened
                timeradd((&start_time),(&temp_time),(&start_time));

                cout<<"Ping found at "<<start_time.tv_sec<<" "<<start_time.tv_usec<<" "<<loop_counter<<endl;

                //Figure out what time it is
                gettimeofday(&curr_time,NULL);

                //Figure out how long ago the ping happened
                timersub((&curr_time), (&start_time), (&temp_time));

                //Now, send data to the main computer
                reportPing(fd,
                        status,
                        ping.direction[0],
                        ping.direction[1],
                        ping.direction[2],
                        (uint16_t) ping.distance,
                        (uint32_t) temp_time.tv_sec*1000+temp_time.tv_usec/1000,
                        (uint32_t) loop_counter);

                //Now find out when we need to wake up
                sleep_time=(NOMINAL_PING_DELAY-(SMALL_DATASET*500)/SAMPRATE)*1000;
                temp_time.tv_sec=int(sleep_time)/int(1000000);
                temp_time.tv_usec=sleep_time%1000000;
                timeradd((&temp_time),(&start_time),(&temp_time));

                while(timercmp((&curr_time), (&temp_time), >)) //check if we are past that point, add sleep intervals until we reach it
                {
                    sleep_time=NOMINAL_PING_DELAY*1000;
                    temp_time.tv_usec+=sleep_time;
                    temp_time.tv_sec+=int(temp_time.tv_usec)/int(1000000);
                    temp_time.tv_usec=int(temp_time.tv_usec)%int(1000000);
                }

                //select() sleeps for the time I have left to sleep, so I need to subtract the current time 
                timersub(&temp_time, &curr_time, &temp_time);
                select(0,NULL,NULL,NULL,&temp_time);
            }
        }
        loop_counter++;
    }while(do_loop);

    closeDevice(fd);
    destroyDataset(dataSet);

    return 0;
}

dataset* getDataset(dataset *dataSet, int status)
{
    int length;  

    if(status<2)
        length=LARGE_DATASET;
    else
        length=SMALL_DATASET;

    if(dataSet==NULL)
    {
        dataSet = createDataset(length);
    }
    else if(dataSet->size != length)
        dataSet->size =length;//change the length.  HAS TO BE SMALLER THAN ORIGINAL

    //greenLightOn();
    captureSamples(dataSet);
    //greenLightOff();

    return dataSet;
}

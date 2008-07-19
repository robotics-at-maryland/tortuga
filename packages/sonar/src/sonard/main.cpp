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
#include <fstream>

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
void getDataset(dataset *dataSet, int length);

int main(int argc, char* argv[])
{
    struct dataset * dataSet = NULL;
    sonarPing ping;
    int ping_found;
    int do_loop=0;
    int loop_counter=0;
    ofstream logfile("sonar_logfile.log");
    struct timeval start_time;
    getDirEdge edge_detector;

    //Initialize the communication port to the main cpu
    //int fd=openDevice();
    int fd=0;

    //Get the starting time
    gettimeofday(&start_time, NULL);
                    //cout<<"Start:"<<start_time.tv_sec<<" "<<start_time.tv_usec<<endl;

    //First, load the initial dataset
    if(argc == 1)
    {
        getDataset(dataSet, LARGE_DATASET);
        do_loop=1; //infinite loop, since I am running off the hydrophones
    }
    else
    {
        logfile<<"Using dataset \n"<<argv[1]<<endl;
        dataSet = loadDataset(argv[1]);
    }

    do
    {
        if(loop_counter!=0) //already loaded the dataset for the first run
        {
            getDataset(dataSet, SMALL_DATASET);
            gettimeofday(&start_time, NULL);
                    //cout<<"Start:"<<start_time.tv_sec<<" "<<start_time.tv_usec<<endl;
        }

        if(dataSet == NULL)
        {
            logfile<<"Could not load dataset!\n";
            //but maybe I'll get luckier the next time?
        }
        else
        {
            if((ping_found=edge_detector.getEdge(&ping, dataSet))==0)
                logfile<<"No ping found\n";
            else if(ping_found!=1)
                logfile<<"Error finding ping \n"<<ping_found<<endl;
            else 
            {
                logfile<<"Vector from pinger: "<<ping.direction[0]<<" "<<ping.direction[1]<<" "<<ping.direction[2]<<endl;

                sleepTillPing(&start_time, ping.point_num);

                //Now, send data to the main computer
                reportPing(fd,
                        0,
                        ping.direction[0],
                        ping.direction[1],
                        ping.direction[2],
                        (uint16_t) ping.distance,
                        (uint32_t) start_time.tv_sec,
                        (uint32_t) start_time.tv_usec);
                //cout<<"Sending "<<fd<<" "<<ping.direction[0]<<" "<<ping.direction[1]<<" "<<ping.direction[2]<<" "<<(uint16_t) ping.distance<<" "<<" "<<(uint32_t) start_time.tv_sec<<" "<<(uint32_t) start_time.tv_usec<<endl;
            }
        }
        loop_counter++;
    }while(do_loop);

    logfile.close();
    closeDevice(fd);

    return 0;
}

void getDataset(dataset *dataSet, int length)
{
    if(dataSet==NULL)
        dataSet = createDataset(length);
    else
        if(dataSet->size != length)
        {
            destroyDataset(dataSet); //delete the previous one to change length
            dataSet = createDataset(length);
        }


    greenLightOn();
    captureSamples(dataSet);
    greenLightOff();
}

/* Calculates how long the program needs to sleep based on the time of ping,
 * current time and the nominal time between pings
 * Sets temp to the time that you want to sleep until
 */

void sleepTillPing(struct timeval *start_time, int point_num)
{
    struct timeval curr_time;
    struct timeval temp;
    int sleep_time;

    //Find how far into the time sample the ping was found
    temp.tv_sec=point_num/SAMPRATE;
    temp.tv_usec=(((point_num-(SAMPRATE*temp.tv_sec))*1000)/SAMPRATE)*1000;

    //Figure out the time at which the ping happened
    timeradd((start_time),(&temp),(start_time));
    //cout<<"Ping happened: "<<start_time->tv_sec<<" "<<start_time->tv_usec<<endl;

    //Figure out what time it is
    gettimeofday(&curr_time,NULL);
    //cout<<"Now:"<<curr_time.tv_sec<<" "<<curr_time.tv_usec<<endl;

    //Now find out when we need to wake up
    sleep_time=(NOMINAL_PING_DELAY-(SMALL_DATASET*500)/SAMPRATE)*1000;
    temp.tv_sec=int(sleep_time)/int(1000000);
    temp.tv_usec=sleep_time%1000000;
    //cout<<"Sleep time: "<<temp.tv_sec<<" "<<temp.tv_usec<<endl;
    timeradd((&temp),(start_time),(&temp));

    //cout<<"Nom. to sleep: "<<temp.tv_sec<<" "<<temp.tv_usec<<endl;
    while(timercmp((&curr_time), (&temp), >)) //check if we are past that point, add sleep intervals until we reach it
    {
        sleep_time=NOMINAL_PING_DELAY*1000;
        temp.tv_usec+=sleep_time;
        temp.tv_sec+=int(temp.tv_usec)/int(1000000);
        temp.tv_usec=int(temp.tv_usec)%int(1000000);
    }

    //select() sleeps for the time I have left to sleep, so I need to subtract the current time 
    //cout<<"Sleeping till: "<<temp.tv_sec<<" "<<temp.tv_usec<<endl;
    timersub(&temp, &curr_time, &temp);
    //cout<<"Sleeping"<<temp.tv_sec<<" "<<temp.tv_usec<<endl;
    //select(0,NULL,NULL,NULL,&temp);
    gettimeofday(&temp,NULL);
    //cout<<"Curr. time"<<temp.tv_sec<<" "<<temp.tv_usec<<endl;
}

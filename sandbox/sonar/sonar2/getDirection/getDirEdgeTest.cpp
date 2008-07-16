//Michael Levashov
//A small program to test the function of the getDirEdge algorithm

#include <stdio.h>
#include <unistd.h>
#include <cmath>

#include "Sonar.h"

#include "spartan.h"
#include "dataset.h"

#include "sonarPing.h"
#include "getDirEdge.h"

using namespace ram::sonar;
using namespace ram::math;

int main(int argc, char* argv[])
{
    //kBands defined in Sonar.h!
    struct dataset * dataSet = NULL;
    sonarPing ping;
    int found;

    if(argc == 1)
    {
        dataSet = createDataset(0xA0000*2);
        if(dataSet == NULL)
        {
            fprintf(stderr, "Could not allocate.\n");
            exit(1);
        }
        REG(ADDR_LED) = 0x02;
        fprintf(stderr, "Recording samples...\n");
        captureSamples(dataSet);
        fprintf(stderr, "Analyzing samples...\n");
        REG(ADDR_LED) = 0x01;
    }
    else
    {
        fprintf(stderr, "Using dataset %s\n", argv[1]);
        dataSet = loadDataset(argv[1]);
    }

    if(dataSet == NULL)
    {
        fprintf(stderr, "Could not load dataset!\n");
        return -1;
    }

    if((found=getDirEdge(&ping, dataSet))==0)
        printf("No ping found\n");
    else if(found!=1)
        printf("Error finding ping %d\n",found);
    else 
    {
        printf("Vector from pinger: %3.3f %3.3f %3.3f\n",ping.direction[0],ping.direction[1],ping.direction[2]);
        printf("Angle: %4.0f\n",180/M_PI*atan2(ping.direction[0],ping.direction[1]));
    } 
    return 0;
}

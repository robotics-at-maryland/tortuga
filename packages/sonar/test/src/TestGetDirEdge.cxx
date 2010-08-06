//Michael Levashov
//Robotics@Maryland
//A small program to test the function of the getDirEdge algorithm

#include <stdio.h>
#include <unistd.h>
#include <cmath>

//#include "math/include/MatrixN.h"
//#include "math/include/Vector3.h"
#include "sonar/include/SonarPing.h"

#include "sonar/include/Sonar.h"

#include "drivers/bfin_spartan/include/dataset.h"
#include "drivers/bfin_spartan/include/spartan.h"

#include "sonar/include/GetDirEdge.h"

using namespace ram::sonar;
using namespace ram::math;

int main(int argc, char* argv[])
{
    //kBands defined in Sonar.h!
    struct dataset * dataSet = NULL;
    sonarPing ping;
    int found;
    
    int myKBands[nKBands];
    for (int i = 0 ; i < nKBands ; i ++)
        myKBands[i] = kBands[i];
    for (int argIndex = 1 ; argIndex < argc ; argIndex ++)
    {
        if (strcmp(argv[argIndex], "--swap-bands") == 0)
        {
            int temp = myKBands[0];
            myKBands[0] = myKBands[1];
            myKBands[1] = temp;
        } else {
            fprintf(stderr, "Using dataset %s\n", argv[argIndex]);
            dataSet = loadDataset(argv[argIndex]);
        }
    }
    if(dataSet == NULL)
    {
        dataSet = createDataset(0xA0000*2);
        if(dataSet == NULL)
        {
            fprintf(stderr, "Could not allocate.\n");
            exit(1);
        }
        //greenLightOn();
        fprintf(stderr, "Recording samples...\n");
        captureSamples(dataSet);
        fprintf(stderr, "Analyzing samples...\n");
        //greenLightOff();
    }
    
    getDirEdge edge_detector(myKBands);

    if(dataSet == NULL)
    {
        fprintf(stderr, "Could not load dataset!\n");
        return -1;
    }

    if((found=edge_detector.getEdge(&ping, dataSet))==0)
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

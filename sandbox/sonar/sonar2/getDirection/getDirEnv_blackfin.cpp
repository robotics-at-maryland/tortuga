//Michael Levashov
//A small program to test the function of the pingDetect algorithm

#include "getDirectionEnvelope.h"
#include "Sonar.h"
#include "sonarPing.h"

#include "dataset.h"
#include "spartan.h"

#include "dataset.c"
#include "spartan.c"

#include <stdio.h>
#include <unistd.h>
#include <cmath>

using namespace ram::sonar;
using namespace ram::math;
using namespace std;

int main(int argc, char* argv[])
{
    sonarPing pings[MAX_PINGS_PER_SET];
    struct dataset * dataSet = NULL;

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
        printf("done loading\n");
    }

    if(dataSet == NULL)
    {
        fprintf(stderr, "Could not load dataset!\n");
        return -1;
    }

    int num_pings=getDirectionEnvelope(dataSet, pings);
    printf("%d pings found\n", num_pings);

//    printf("Directions:\n");
 //   for(int i=0; i<num_pings; i++)
  //      printf("Ping %3.3f %3.3f %3.3f",dataSet[i].directions[0],dataSet[i].directions[1],dataSet[i].directions[2]);

    return 0;
}

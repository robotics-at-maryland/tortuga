//Michael Levashov
//A small program to test the function of the pingDetect algorithm
//

#include <stdio.h>
#include <unistd.h>

#include "Sonar.h"

#include "spartan.h"
#include "dataset.h"

#include "sonarPing.h"
#include "getDirEnvelope.h"

using namespace ram::sonar;
//using namespace ram::math;

int main(int argc, char* argv[])
{
    //kBands defined in Sonar.h!
    struct dataset * dataSet = NULL;
    sonarPing ping;

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

    getDirEnvelope(&ping, dataSet);

    return 0;
}

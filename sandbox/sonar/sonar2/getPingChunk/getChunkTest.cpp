//Michael Levashov
//A small program to test the function of the pingDetect algorithm
//

#include <stdio.h>
#include <unistd.h>

#include "Sonar.h"

#include "spartan.h"
#include "dataset.h"

#include "SparseSDFTSpectrum.h"
#include "getPingChunk.h"

using namespace ram::sonar;
//using namespace ram::math;

int main(int argc, char* argv[])
{
    //kBands defined in Sonar.h!
    struct dataset * dataSet = NULL;
    adcdata_t* data[NCHANNELS];
    int locations[NCHANNELS];
    FILE* f;

    for(int j=0; j<NCHANNELS; j++)
        data[j]=new adcdata_t(ENV_CALC_FRAME);

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

    getPingChunk(data, locations, dataSet);

    f=fopen("out_file.bin","w");
    if (f==NULL)
    {
        fprintf(stderr,"Could not open output file\n");
        return -1;
    }

    //Now, print it
    //for(int j=0; j<ENV_CALC_FRAME; j++)
        //for(int i=0; i<NCHANNELS; i++)
            //fwrite(&data[i][j], sizeof(adcdata_t),1, f);

    for(int i=0; i<NCHANNELS; i++)
    {
        printf("Location %d: %d\n",i,locations[i]);
        //delete data[i];
    }

    fclose(f);

    return 0;
}

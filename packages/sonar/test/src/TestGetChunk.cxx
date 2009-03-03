//Michael Levashov
//Robotics@Maryland
//
//A small program to test the function of the pingDetect algorithm
//

#include <stdio.h>
#include <unistd.h>

#include "sonar/include/Sonar.h"

#include "drivers/bfin_spartan/include/dataset.h"
#include "drivers/bfin_spartan/include/spartan.h"

#include "sonar/include/GetPingChunk.h"

using namespace ram::sonar;
//using namespace ram::math;

int main(int argc, char* argv[])
{
    struct dataset * dataSet = NULL;
    adcdata_t* data[NCHANNELS];
    int locations[NCHANNELS];
    
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

    getPingChunk chunk(myKBands);
    FILE* f;

    for(int j=0; j<NCHANNELS; j++)
        data[j]=new adcdata_t [ENV_CALC_FRAME];

    if(dataSet == NULL)
    {
        fprintf(stderr, "Could not load dataset!\n");
        return -1;
    }

    int result = chunk.getChunk(data, locations, dataSet);

    if (result != 1)
    {
        fprintf(stderr, "Did not find a ping!\n");
        return -1;
    }
    
    f=fopen("out_file.bin","w");
    if (f==NULL)
    {
        fprintf(stderr,"Could not open output file\n");
        return -1;
    }

    //Now, print it
    for(int j=0; j<ENV_CALC_FRAME; j++)
    {
        for(int i=0; i<NCHANNELS; i++)
        {
	  if(sizeof(adcdata_t) != 
	     fwrite(&data[i][j], sizeof(adcdata_t),1, f))
          {
	    fprintf(stderr,"Could not write to output file\n");
	    return -1;
	  }
	}
    }

    for(int i=0; i<NCHANNELS; i++)
    {
        printf("Location %d: %d\n",i,locations[i]);
        //delete data[i];
    }

    fclose(f);

    return 0;
}

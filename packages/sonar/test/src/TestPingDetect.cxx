//Michael Levashov
//Robotics@Maryland
//
//A small program to test the function of the pingDetect algorithm
//

#include <iostream>
#include <unistd.h>
#include <cmath>
#include <complex>

#include "sonar/include/Sonar.h"
//#include "SparseSDFTSpectrum.h"
#include "sonar/include/PingDetect.h"

#include "sonar/include/fixed/fixed.h"
#include "math/include/MatrixN.h"
#include "math/include/Vector3.h"

#include "drivers/bfin_spartan/include/dataset.h"
#include "drivers/bfin_spartan/include/spartan.h"

using namespace ram::sonar;
using namespace ram::math;
using namespace std;

//MatrixN hydroStructure(*hydroStructureArray, 3, 3);

int main(int argc, char* argv[])
{
    //kBands defined in Sonar.h!
    int thresholds[]={50,50,50,50};
    pingDetect pdetect(thresholds, kBands,PING_DETECT_FRAME);
    struct dataset * dataSet = NULL;
    int lastDetectedIndex = 0;
    int lastPingIndex[NCHANNELS]={0,0,0,0};
    std::bitset<NCHANNELS> lastDetectedFlag;

    if(argc == 1)
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
    for(int i=0; i<dataSet->size; i++)
    {
        adcdata_t sample[NCHANNELS];
        for (int channel = 0 ; channel < NCHANNELS ; channel++)
            sample[channel] = getSample(dataSet, channel, i);
        const std::bitset<NCHANNELS> detectedFlag = pdetect.p_update(sample);

        if (i == DFT_FRAME)
            pdetect.reset_minmax();

        if (i - lastDetectedIndex > MAX_PING_SEP)
        {
            lastDetectedFlag.reset();
            lastDetectedIndex = 0;
        }
        
        if (detectedFlag.any())
        {
            lastDetectedIndex = i;
            for (int channel = 0 ; channel < NCHANNELS ; channel ++)
            {
                if (detectedFlag[channel] && !lastDetectedFlag[channel])
                {
                    std::cout << "Hydrophone " << channel 
                              << " detected a ping at point " 
                              << i + 1 << std::endl;
                    lastDetectedFlag.set(channel);
                    lastPingIndex[channel] = i;
                }
            }
        }

        if (lastDetectedFlag.count() == NCHANNELS)
        {
            std::cout << "Ping detected at " << i+1-PING_DETECT_FRAME/2 << "!" << std::endl;
            pdetect.reset_minmax();
            lastDetectedFlag.reset();
        }
    }

    return 0;
}

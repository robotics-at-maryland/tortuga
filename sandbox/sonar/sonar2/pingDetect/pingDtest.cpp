//Michael Levashov
//A small program to test the function of the pingDetect algorithm
//

#include <iostream>
#include <unistd.h>
#include <cmath>
#include <complex>

#include "Sonar.h"
#include "SparseSDFTSpectrum.h"
#include "pingDetect.h"

#include "fixed.h"
#include "math/include/MatrixN.h"
#include "math/include/Vector3.h"

#include "spartan.h"
#include "dataset.h"

//using namespace ram::sonar;
//using namespace ram::math;
//using namespace std;

ram::math::MatrixN hydroStructure(*ram::sonar::hydroStructureArray, 3, 3);

int main(int argc, char* argv[])
{
    //kBands defined in Sonar.h!
    int thresholds[]={20,20,20,20};
    int detected;
    ram::sonar::pingDetect pdetect(thresholds, ram::sonar::NCHANNELS, ram::sonar::kBands);
    ram::sonar::adcdata_t sample[ram::sonar::NCHANNELS];
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
    }

    if(dataSet == NULL)
    {
        fprintf(stderr, "Could not load dataset!\n");
        return -1;
    }
    for(int i=0; i<dataSet->size; i++)
    {
        sample[0] = getSample(dataSet, 0, i);
        sample[1] = getSample(dataSet, 1, i);
        sample[2] = getSample(dataSet, 2, i);
        sample[3] = getSample(dataSet, 3, i);
        detected=pdetect.p_update(sample,0);

        if(detected != 0)
        {
            if((detected & 1) != 0)
                cout<<"Hydrophone 0 detected a ping at point "<<i+1<<endl;
            if((detected & 2) != 0)
                cout<<"Hydrophone 1 detected a ping at point "<<i+1<<endl;
            if((detected & 4) != 0)
                cout<<"Hydrophone 2 detected a ping at point "<<i+1<<endl;
            if((detected & 8) != 0)
                cout<<"Hydrophone 3 detected a ping at point "<<i+1<<endl;
        }

        if(detected==15)
        {
            cout<<"Ping detected at "<<i+1-ram::sonar::PING_DETECT_FRAME/2<<"!\n";
            pdetect.reset_minmax();
        }
    }

    return 0;
}

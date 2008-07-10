//Michael Levashov
//A small program to test the function of the pingDetect algorithm
//
#include "getDirectionEnvelope.h"
#include "Sonar.h"
#include "pingDetect.h"
#include "circArray.h"

#include <iostream>
#include <unistd.h>
#include <cmath>

//#include "fixed.h"
//#include "math/include/MatrixN.h"
//#include "math/src/MatrixN.cpp"

#include "dataset.h"
#include "spartan.h"

using namespace ram::math;
using namespace std;

namespace ram {
namespace sonar {
int getDirectionEnvelope(struct dataset *dataSet, sonarPing* pings)
{
    int numpoints=0;
    int phasecalcPoint=-1;
    int sleepDetectPoint=-1;
    int thresholds[]={30,30,30,30};
    int detected;
    int numpings=0;
    double phase[NCHANNELS];
    circArray<adcdata_t,ENV_CALC_FRAME> data_array[NCHANNELS];
    pingDetect pdetect(thresholds, NCHANNELS, kBands);
    adcdata_t sample[NCHANNELS];

    FILE* f=NULL;

    cout<<"Dataset size: "<<dataSet->size<<endl;

    for(int i=0; i<dataSet->size; i++)
    {
        sample[0] = getSample(dataSet, 0, i);
        sample[1] = getSample(dataSet, 1, i);
        sample[2] = getSample(dataSet, 2, i);
        sample[3] = getSample(dataSet, 3, i);
        data_array[0].update(sample[0]);
        data_array[1].update(sample[1]);
        data_array[2].update(sample[2]);
        data_array[3].update(sample[3]);
        numpoints++;

        detected=pdetect.p_update(sample,0);

        if((detected==15) && (numpoints>sleepDetectPoint))
        {
            numpings++;
            if(numpings > MAX_PINGS_PER_SET)
            {
                cout<<"Too many pings detected!\n";
                return -1;
            }
            cout<<"Ping "<<numpings<<"detected at "<<numpoints-PING_DETECT_FRAME/2<<"!\n";
            pdetect.reset_minmax();

            //Now, figure out how long we need to wait until we want to calculate the phase
            phasecalcPoint=numpoints+ENV_CALC_FRAME/2;
            sleepDetectPoint=numpoints+DETECT_SLEEP_TIME*SAMPRATE;
            pings[numpings].point_num = numpoints;
        }
        //Calculate phase after detecting the ping
        else if(phasecalcPoint==numpoints)
        {
            if(numpings==1)
                f = fopen("ping_chunk1.bin", "w");
            else if(numpings==2)
                f = fopen("ping_chunk2.bin", "w");
            else if(numpings==3)
                f = fopen("ping_chunk3.bin", "w");

            for(int i=0; i<ENV_CALC_FRAME; i++)
                for(int j=0; j<NCHANNELS; j++)
                    fwrite(*(data_array[j])+i*sizeof(adcdata_t), sizeof(adcdata_t),1,f);
        }
    }

    if(f!=NULL)
        fclose(f);

    return numpings;
}

} //namespace sonar
} //namespace ram

//Michael Levashov
//A small program to test the function of the pingDetect algorithm
//
#include "getDirectionPhase.h"
#include "Sonar.h"
#include "pingDetect.h"

#include <iostream>
#include <unistd.h>
#include <cmath>

#include "fixed.h"
#include "math/include/MatrixN.h"
#include "math/include/Vector3.h"
#include "math/src/MatrixN.cpp"

#include "dataset.h"
#include "spartan.h"

#include "spartan.c"
#include "dataset.c"

using namespace ram::sonar;
using namespace ram::math;
using namespace std;

int getDirectionPhase(struct dataset *dataSet, sonarPing* pings)
{
    int numpoints=0;
    int phasecalcPoint=-1;
    int sleepDetectPoint=-1;
    int thresholds[]={20,20,20,20};
    int detected;
    int numpings=0;
    double phase[NCHANNELS];
    pingDetect pdetect(thresholds, NCHANNELS, kBands);
    adcdata_t sample[NCHANNELS];
    MatrixN hydroStructure(*hydroStructureArray, 3, 3);
    hydroStructure.invert();

    for(int i=0; i<dataSet->size; i++)
    {
        sample[0] = getSample(dataSet, 0, i);
        sample[1] = getSample(dataSet, 1, i);
        sample[2] = getSample(dataSet, 2, i);
        sample[3] = getSample(dataSet, 3, i);
        numpoints++;
        detected=pdetect.p_update(sample,0);

        if((detected==15) && (numpoints>sleepDetectPoint))
        {
            numpings++;
            if(numpings > MAX_PINGS_PER_SET)
            {
                cout<<"Too many pings detected!\n"
                return -1;
            }
            cout<<"Ping "<<numpings<<"detected at "<<numpoints-PING_DETECT_FRAME/2<<"!\n";
            pdetect.reset_minmax();

            //Now, figure out how long we need to wait until we want to calculate the phase
            phasecalcPoint=numpoints-PING_DETECT_FRAME/2+DFT_FRAME/2-50;
            sleepDetectPoint=numpoints+DETECT_SLEEP_TIME*SAMPRATE;
            pings[numpings]->point_num = numpoints;
        }
        //Calculate phase after detecting the ping
        else if(phasecalcPoint==numpoints)
        {
            phase[0] = pdetect.get_phase(0, 0);
            MatrixN tdoas(3, 1);
    
            for (int channel = 1 ; channel < NCHANNELS ; channel ++)
            {
                phase[channel] = pdetect.get_phase(0, channel);
                cout<<channel<<" Phase: "<<phase[channel]<<endl;
                tdoas[channel - 1][0] = phase[0] - phase[channel];

                if((tdoas[channel-1][0]) > M_PI)
                    tdoas[channel-1][0]-=M_PI*2;
                else if((tdoas[channel -1][0]) < -M_PI)
                    tdoas[channel-1][0]+=M_PI*2;
            }

            MatrixN direction = hydroStructure * tdoas;

            for (int i = 0 ; i < 3 ; i ++)
                pings[numpings]->direction[i] = direction[i][0];

            pings[numpings]->directior.normalise();
        }
    }

    return numpings;
}

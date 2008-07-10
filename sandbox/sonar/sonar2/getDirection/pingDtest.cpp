//Michael Levashov
//A small program to test the function of the getDirectionEnvelope algorithm

#include "getDirectionEnvelope.h"
#include "Sonar.h"
#include "sonarPing.h"

#include "dataset.h"
#include "dataset.c"
#include "spartan.h"
#include "spartan.c"

#include <stdio>
#include <unistd.h>
#include <cmath>

using namespace ram::sonar;
using namespace ram::math;
using namespace std;

int main(argc, char* argv[])
{
    sonarPing ping[MAX_PINGS_PER_SET];
    struct dataset * dataSet = NULL;

    hydroStructure.invert();
    if(argc < 2)
    {
        cout<<"Use: pingDtest filename\n";
        cout<<"File should have int16 data for "<<NCHANNELS<<" channels\n";
        exit(-1);
    }

    FILE* dfile = fopen (argv[1], "r");
    if (dfile == NULL)
    {
        cerr<<"Error opening file "<<argv[0]<<endl;
        exit(-1);
    }

    int num_pings=getDirectionPhase(&dataSet, &pings);
    printf("%d pings found\n", num_pings);

    printf("Directions:\n");
    for(int i=0; i<num_pings; i++)
        printf("Ping %3.3f %3.3f %3.3f",dataSet[i].directions[0],dataSet[i].directions[1],dataSet[i].directions[2]);

    return 0;
}

//Michael Levashov
//A small program to test the function of the pingDetect algorithm

#include "pingDetect.h"
#include "Sonar.h"
#include <iostream>
#include <unistd.h>
#include <cmath>

#include "fixed.h"
#include "math/include/MatrixN.h"
#include "math/include/Vector3.h"

#include "dataset.h"
#include "spartan.h"

#include "spartan.c"
#include "dataset.c"

//static const double hydroStructureArray[3][3] =
//{
    //{0,      0.984, 0,   },
    //{0.492,  0.492, 0.696},
    //{-0.492, 0.492, 0.696}
//};


static const double hydroStructureArray[3][3] =
{
    {0,      1.0163, -1.0163,   },
    {1.0163,  0, 0},
    {-0.7184, 0.7184, 0.7184}
};

using namespace ram::sonar;
using namespace ram::math;
using namespace std;

MatrixN hydroStructure(*hydroStructureArray, 3, 3);

int main(int argc, char* argv[])
{

    int numchan=4;
    int numpoints=0;
    int phasecalcPoint=-1;
    int phasecalcChannel=0;
    int thresholds[]={20,20,20,20};
    int detected;
    double phase[NCHANNELS];
    pingDetect pdetect(thresholds, numchan, kBands);
    adcdata_t sample[numchan];
    struct dataset * dataSet = NULL;
    //hydroStructure.invert();

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
        numpoints++;
        detected=pdetect.p_update(sample,0);

        /*if(detected != 0)
        {
            if((detected & 1) != 0)
                //cout<<"Hydrophone 0 detected a ping at point "<<numpoints<<endl;
            if((detected & 2) != 0)
                //cout<<"Hydrophone 1 detected a ping at point "<<numpoints<<endl;
            if((detected & 4) != 0)
                //cout<<"Hydrophone 2 detected a ping at point "<<numpoints<<endl;
            if((detected & 8) != 0)
                //cout<<"Hydrophone 3 detected a ping at point "<<numpoints<<endl;
        }*/

        if(detected==15)
        {
            cout<<"Ping detected at "<<numpoints-PING_DETECT_FRAME/2<<"!\n";
            pdetect.reset_minmax();

            //Now, figure out how long we need to wait until we want to calculate the phase
            phasecalcPoint=numpoints-PING_DETECT_FRAME/2+DFT_FRAME/2+100;
        }
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
            Vector3 directionVector;

            for (int i = 0 ; i < 3 ; i ++)
                directionVector[i] = direction[i][0];

            directionVector.normalise();
            cerr << "Direction to pinger is: " << directionVector << endl;

            float yaw = 180*atan2(directionVector.y, directionVector.x) / M_PI;
            if(yaw < 0)
                yaw += 360;

            cerr << "Yaw is "<<yaw<<endl;

            //cout<<"Calculating at "<<numpoints<<endl;
            //for(int k=0; k<numchan;k++)
            //{
                //phase[k]=pdetect.get_phase(k,0);
                //cout<<k<<" Phase: "<<phase[k]<<endl;
            //}
            //for(int k=1; k<numchan;k++)
                //cout<<k<<" DPhase: "<<phase[k]-phase[0]<<endl;
        }
    }

    //fclose(dfile);

    return 0;
}

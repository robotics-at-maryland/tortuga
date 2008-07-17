/* Michael Levashov
 * Robotics@Maryland
 *
 * This function is responsible for finding the pings in a chunk of data and filling an array with data that contains the pings 
 * It tries to be smart and pick out the chunks appropriate for each array,
 * while ignoring false positives that are far apart
 * data is an NCHANNEL-sized array of pointers to ENV_CALC_FRAME-sized int arrays
 * The pings are extracted from the dataSet and are stored there
 * locations is a pointer to an NCHANNEL-sized array of integers storing the locations of the first points in the data arrays with respect to the dataSet, starting at 0
 */

#include <iostream>
#include <unistd.h>
#include <cmath>
#include <complex>

#include "Sonar.h"
#include "SparseSDFTSpectrum.h"
#include "pingDetect.h"

#include "fixed.h"

#include "spartan.h"
#include "dataset.h"

#include "getPingChunk.h"

//using namespace std;

namespace ram {
namespace sonar {

int getPingChunk(adcdata_t** data, int* locations, struct dataset* dataSet)
{
    //kBands defined in Sonar.h!
    int detected;
    pingDetect pdetect=pingDetect(PD_THRESHOLDS, NCHANNELS, kBands, PING_DETECT_FRAME);
    adcdata_t sample[NCHANNELS];
    int last_detected=0;
    int last_ping_index[NCHANNELS]={0,0,0,0};
    int last_value[NCHANNELS]={0, 0, 0, 0};

    for(int i=0; i<dataSet->size; i++)
    {
        sample[0] = getSample(dataSet, 0, i);
        sample[1] = getSample(dataSet, 1, i);
        sample[2] = getSample(dataSet, 2, i);
        sample[3] = getSample(dataSet, 3, i);

        detected=pdetect.p_update(sample,0);

        if(i==DFT_FRAME)
            pdetect.reset_minmax();

        if(i-last_detected>MAX_PING_SEP)
        {
            for(int j=0; j<NCHANNELS; j++)
                last_value[j]=0;
            last_detected=0;
        }

        if(detected !=0)
        {
            last_detected=i;
            if(((detected & 1) != 0) && (last_value[0]!=1))
            {
                //cout<<"Hydrophone 0 detected a ping at point "<<i+1<<endl;
                last_value[0]=1;
                last_ping_index[0]=i;
            }
            if(((detected & 2) != 0) && (last_value[1]!=1))
            {
                //cout<<"Hydrophone 1 detected a ping at point "<<i+1<<endl;
                last_value[1]=1;
                last_ping_index[1]=i;
            }
            if(((detected & 4) != 0) && (last_value[2]!=1))
            {
                //cout<<"Hydrophone 2 detected a ping at point "<<i+1<<endl;
                last_value[2]=1;
                last_ping_index[2]=i;
            }
            if(((detected & 8) != 0) && (last_value[3]!=1))
            {
                //cout<<"Hydrophone 3 detected a ping at point "<<i+1<<endl;
                last_value[3]=1;
                last_ping_index[3]=i;
            }
        }

        if((last_value[0]==1) &&
           (last_value[1]==1) &&
           (last_value[2]==1) &&
           (last_value[3]==1))
        {
            cout<<"Ping detected at "<<i+1<<"!\n";
            if((last_ping_index[0]<ENV_CALC_FRAME) || //too close to the start, skip it
               (last_ping_index[1]<ENV_CALC_FRAME) || 
               (last_ping_index[2]<ENV_CALC_FRAME) || 
               (last_ping_index[3]<ENV_CALC_FRAME))
            {
                pdetect.reset_minmax();
                for(int j=0; j<NCHANNELS; j++)
                    last_value[j]=0;
            }
            else
            {
                for(int j=0; j<NCHANNELS; j++)
                {
                    for(int k=0; k<ENV_CALC_FRAME; k++)
                        data[j][k]=getSample(dataSet, j, k+last_ping_index[j]-ENV_CALC_FRAME+1+DFT_FRAME/2); //might need to be tweaked
                    locations[j]=last_ping_index[j]-ENV_CALC_FRAME+1;
                }

                return 1;
            }
        }
    }
    return 0;
}

}//sonar
}//ram

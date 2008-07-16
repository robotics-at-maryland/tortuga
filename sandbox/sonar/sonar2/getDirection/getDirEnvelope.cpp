/* Michael Levashov
 * Robotics@Maryland
 * A function to calculate the direction that the ping is coming from
 * given a set of data.
 * First, it runs a threshold-based ping-finding algorithm pingDetect.
 * getPingChunk returns it chunks of data containing the ping.
 * Then it runs time_diff on the data, which finds the time offset using
 * fft convolutions.
 * Finally, the times are used to calculate the direction and distance to the pinger
 * The function returns 1 if it finds the ping, 0 if not, -1 if error
 */

#include <unistd.h>
#include <stdio.h>
#include "Sonar.h"

#include "spartan.h"
#include "dataset.h"

#include "SparseSDFTSpectrum.h"
#include "getPingChunk.h"

#include "fft_sonar.h"
#include "sonar_quadfit.h"
#include "time_diff.h"

#include "sonarPing.h"

#include "getDirEnvelope.h"

namespace ram {
namespace sonar {

int getDirEnvelope(sonarPing* ping, dataset* dataSet)
{
    adcdata_t* data[NCHANNELS];
    int locations[NCHANNELS];
    int found_ping;
    double tdiff; //time difference between two signals
    int tdoas[NCHANNELS][NCHANNELS]; //a redundant array to store tdoas.  Fix later.  Tdoa's go from the 1st to the 2nd

    time_diff* mytdiff=new time_diff(NCHANNELS, ENV_CALC_FRAME, frequencyOfInterest, (int)frequencyOfInterest*0.9, (int)frequencyOfInterest*1.1); //a class that calculates time differences

    for(int j=0; j<NCHANNELS; j++)
        data[j]=new adcdata_t(ENV_CALC_FRAME);

    //Analyze the data, extract the relevant chunks
    found_ping=getPingChunk(data, locations, dataSet);
    if(found_ping!=1)
        return found_ping; //couldn't find or error

    //Now, I update the time_diff class with new data:
    if(mytdiff->update(&data[0])!=0)
        return -1;  //failed to update date sample, something is wrong with the transforms

    for(int k=0; k<NCHANNELS-1;k++)
        for(int j=k+1;j<NCHANNELS;j++)
            if(mytdiff->calc_time_diff(k,j,tdiff)!=0) //Screwed up getting time difference.  The point is no use, I might as wel quit
            {
                printf("Error calculating tdiff!\n");
                return -2;
            }
            else
            {
                if(tdiff>ENV_CALC_FRAME/2) //bring the point to within half of the frame
                    tdiff-=ENV_CALC_FRAME;
                printf("Between %d and %d: %f points\n",k+1,j+1,tdiff);
                tdoas[k][j]=tdiff+(locations[k]-locations[j]);//CHECK SIGN!!!!!!!!!
                tdoas[j][k]=-tdoas[k][j]; //might as well store the other one, too
            }
    delete mytdiff;
    for(int i=0; i<NCHANNELS; i++)
        delete data[i];

    return 1;
}
} //sonar
} //ram

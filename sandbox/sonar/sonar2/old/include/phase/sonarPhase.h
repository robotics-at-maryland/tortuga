//Michael Levashov
//Leo's algorithm to calculate the phases

#ifndef _RAM_SONAR_PHASE
#define _RAM_SONAR_PHASE

#include "pingDetect.h"
#include <iostream>

namespace ram {
namespace sonar {


class sonarPhase
{
    int numchan;
    int numpoints;
    int detected;
    adcdata_t sample[NCHANNELS];
    pingDetect *pdetect;

    class sonarPhase(int* thresholds, numchan)
    {
        pdetect= new pingDetect(thresholds, numchan);

    while(fread(sample,sizeof(adcdata_t),numchan,dfile) == (size_t)numchan)
    {
        numpoints++;
        detected=pdetect.update(sample);

        if(detected==15)
        {
            cout<<"Ping detected at "<<numpoints-PING_DETECT_FRAME/2<<"!\n";
            pdetect.reset_minmax();
        }
    }

    fclose(dfile);

    return 0;
} //sonar
} //ram
#endif

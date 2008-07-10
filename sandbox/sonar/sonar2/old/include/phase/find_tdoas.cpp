//Michael Levashov
//A small program to test the function of the pingDetect algorithm
//Tests an algorithm to check the phases

#include "pingDetect.h"
#include <iostream>
#include <unistd.h>

using namespace ram::sonar;
using namespace std;

int main(int argc, char* argv[])
{
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

    int numchan=4;
    int numpoints=0;
    int thresholds[]={10,10,10,10};
    int detected;
    pingDetect pdetect(thresholds, numchan);
    adcdata_t sample[numchan];
    int numdet=0;

    while(fread(sample,sizeof(adcdata_t),numchan,dfile) == (size_t)numchan)
    {
        numdet=0;
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
}

#include <iostream>
#include <Ice/Ice.h>
#include <portaudio.h>
#include <marcopolo.h>

#define SAMPLE_RATE 44100

using namespace std;

short lastValue = 0;
bool risingEdge = true;
short triggerLevel = 400;
::ram::marcopolo::PoloPrx polo;
Ice::Short snapshot[480];
::ram::marcopolo::Snapshot snap;

class MarcoI : public ram::marcopolo::Marco
{
public:
    virtual void setChannelEnabled(::Ice::Int, bool, const ::Ice::Current& = ::Ice::Current())
    {
    }
    
    virtual bool getChannelEnabled(::Ice::Int, const ::Ice::Current& = ::Ice::Current())
    {
        return false;
    }
    
    virtual void setTriggerChannel(::Ice::Int channel, const ::Ice::Current& = ::Ice::Current())
    {
    }
    
    virtual void incrementTriggerLevel(::Ice::Short amount, const ::Ice::Current& = ::Ice::Current())
    {
        triggerLevel += amount;
        if (polo)
            polo->triggerChanged(triggerLevel);
    }
    
    virtual void decrementTriggerLevel(::Ice::Short amount, const ::Ice::Current& = ::Ice::Current())
    {
        triggerLevel -= amount;
        if (polo)
            polo->triggerChanged(triggerLevel);
    }
    
    virtual void setTriggerEdge(bool rising, const ::Ice::Current& = ::Ice::Current())
    {
        risingEdge = rising;
    }
    
    virtual void registerPolo(const ::ram::marcopolo::PoloPrx& p, const ::Ice::Current& = ::Ice::Current())
    {
        polo = p->ice_oneway();
    }
    
};

bool triggered = false;
int snapshotIndex = 0;

int paAudioReceivedCallback(const void* inputBuffer,
                            void* output,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void* userData)
{
    short* audioIn = (short*)inputBuffer;
    for (int i = 0 ; i < framesPerBuffer ; i ++, audioIn = &audioIn[1])
    {
        Ice::Short* sample = &snapshot[snapshotIndex];
        sample[0] = audioIn[0];
        
        if (triggered)
        {
            if (++snapshotIndex >= 480)
            {
                triggered = false;
                snapshotIndex = 0;
                
                try {
                    if (polo)
                    {
                        for (int j = 0 ; j < 480 ; j ++)
                            snap.samples[j] = snapshot[j];
                        snap.triggerLevel = triggerLevel;
                        polo->reportData(snap);
                    }
                } catch (const Ice::Exception& ex) {
                    cerr << ex << endl;
                }
        }
        } else {
            Ice::Short triggerValue = sample[0];
            if ((risingEdge && triggerValue > triggerLevel && lastValue <= triggerLevel) || (!risingEdge && triggerValue < triggerLevel && lastValue >= triggerLevel))
            {
                triggered = true;
                snapshotIndex = 0;
            }
        }
        lastValue = sample[0];
    }
    return 0;
}

int
main(int argc, char* argv[])
{
    snap.samples = ::ram::marcopolo::ShortList(480);
    PaStream* paStream;
    
    int status = 0;
    Ice::CommunicatorPtr ic;
    try {
        PaError paErr;
        // Initialize PortAudio
        paErr = Pa_Initialize();
        if (paErr != paNoError) throw paErr;
        
        // Open PortAudio stream
        paErr = Pa_OpenDefaultStream(&paStream, 1, 0, paInt16, SAMPLE_RATE,
                                     paFramesPerBufferUnspecified,
                                     paAudioReceivedCallback, NULL);
        if (paErr != paNoError) throw paErr;
        
        // Start PortAudio
        paErr = Pa_StartStream(paStream);
        if (paErr != paNoError) throw paErr;
        
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter
		= ic->createObjectAdapterWithEndpoints("MarcoAdapter", "default -p 10000");
        Ice::ObjectPtr object = new MarcoI;
        adapter->add(object, ic->stringToIdentity("Marco"));
        adapter->activate();
        ic->waitForShutdown();
        
        paErr = Pa_StopStream(paStream);
        if (paErr != paNoError) throw paErr;
        
    } catch (const Ice::Exception& e) {
        cerr << e << endl;
        status = 1;
    } catch (PaError pr) {
        cerr << Pa_GetErrorText(pr) << endl;
        status = 1;
    } catch (const char* msg) {
        cerr << msg << endl;
        status = 1;
    }
    
    Pa_Terminate();
    
    if (ic) {
        try {
            ic->destroy();
        } catch (const Ice::Exception& e) {
            cerr << e << endl;
            status = 1;
        }
    }
    return status;
}

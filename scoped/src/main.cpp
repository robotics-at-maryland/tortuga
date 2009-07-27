#include <iostream>
#include <IceE/IceE.h>
#include <stdint.h>
#include <sys/time.h>
#include <string.h>
#include <cmath>
#include "scope.h"

using namespace std;

class OscilloscopeImpl : virtual public ram::sonar::scope::Oscilloscope {
private:
    bool hasViewer;
    ram::sonar::scope::ViewerPrx viewerPrx;
    
    int iHoldoff;
    int iSkip;
    
    short horizontalZoom;
    short triggerChannel;
    short triggerLevel;
    int triggerHoldoff;
    ::ram::sonar::scope::TriggerSlope triggerSlope;
    
    
    static suseconds_t getMicroseconds()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
    }
    
    bool isSampleAvailable()
    {
        static suseconds_t lastTriggered = getMicroseconds();
        
        suseconds_t now = getMicroseconds();
        if (now - lastTriggered > 100)
        {
            lastTriggered = now;
            return true;
        } else
            return false;
    }
    
    void getSample(int16_t sample[4])
    {
        static uint64_t sampleCount = 0;
        ++sampleCount;
        
        int16_t val;
        if ((sampleCount / 1000) % 20 == 0)
            val = sin(2 * M_PI * sampleCount * 20000);
        else
            val = 0;
        
        for (int i = 0 ; i < 4 ; i ++)
            sample[i] = val;
    }
    
    
    
    uint16_t iBuf;
    static const unsigned int LOG2_BUFSIZE = 12;
    static const unsigned int BUFSIZE = 1 << LOG2_BUFSIZE;
    int16_t buf[BUFSIZE][4];
public:
    
    OscilloscopeImpl() :
    hasViewer(false), horizontalZoom(0),
    iHoldoff(0), iSkip(0), iBuf(0),
    triggerChannel(0), triggerLevel(0), triggerHoldoff(0),
    triggerSlope(::ram::sonar::scope::TriggerSlopeRising) {}
    
    virtual void SetViewer(const ::ram::sonar::scope::ViewerPrx& viewerPrx, const ::Ice::Current&)
    {
        this->viewerPrx = viewerPrx;
        hasViewer = true;
    }
    
    virtual void SetTriggerMode(::ram::sonar::scope::TriggerMode, const ::Ice::Current&)
    {
    }
    
    virtual void SetTriggerChannel(::Ice::Short triggerChannel, const ::Ice::Current&)
    {
        this->triggerChannel = triggerChannel;
    }
    
    virtual void SetTriggerLevel(::Ice::Short triggerLevel, const ::Ice::Current&)
    {
        this->triggerLevel = triggerLevel;
    }
    
    virtual void SetTriggerHoldoff(::Ice::Int triggerHoldoff, const ::Ice::Current&)
    {
        this->triggerHoldoff = triggerHoldoff;
    }
    
    virtual void SetTriggerSlope(::ram::sonar::scope::TriggerSlope triggerSlope, const ::Ice::Current&)
    {
        this->triggerSlope = triggerSlope;
    }
    
    virtual void SetHorizontalPosition(::Ice::Int, const ::Ice::Current&)
    {
    }
    
    virtual void SetHorizontalZoom(::Ice::Short zoom, const ::Ice::Current&)
    {
        horizontalZoom = zoom;
    }
    
    virtual ::ram::sonar::scope::OscilloscopeCapture GetLastCapture(const ::Ice::Current&)
    {
    }
    
    void run()
    {
        while (true)
        {
            // Busy wait until a sample is available
            while (!isSampleAvailable())
                ;
            
            // Read new sample
            int16_t sample[4];
            getSample(sample);

            if (iHoldoff > 0)
                --iHoldoff;
            
            ++iSkip;
            
            int16_t lastValueForTrigger = buf[iBuf][triggerChannel];
            
            ++iBuf;
            
            if (iBuf >= LOG2_BUFSIZE)
                iBuf = 0;
            
            if (iSkip >= (1 << horizontalZoom))
            {
                // use this sample and reset the skip counter
                iSkip = 0;
                
                memcpy(buf[iBuf], sample, sizeof(sample));
                
                if (iHoldoff == 0)
                {
                    int16_t thisValueForTrigger = buf[iBuf][triggerChannel];
                    
                    int16_t trigLev = triggerLevel;
                    if (
                        (triggerSlope == ::ram::sonar::scope::TriggerSlopeRising &&
                         lastValueForTrigger < trigLev &&
                         thisValueForTrigger >= trigLev) ||
                        (lastValueForTrigger > trigLev &&
                         thisValueForTrigger <= trigLev)
                        )
                    {
                        // trigger occurred
                        
                    }
                }
            }
            
        }
    }
    
};


int main(int argc, char* argv[])
{
    int status = 0;
    Ice::CommunicatorPtr ic;
    
    try {
        
        ic = Ice::initialize(argc, argv);
        
        Ice::ObjectAdapterPtr adapter = ic->createObjectAdapterWithEndpoints(
            "scoped", "default -p 10000");
        
        IceUtil::Handle<OscilloscopeImpl> osc = new OscilloscopeImpl;
        adapter->add(osc, ic->stringToIdentity("osc"));
        adapter->activate();
        osc->run();
        ic->waitForShutdown();
        
    } catch (const Ice::Exception& e) {
        cerr << e << endl;
        status = 1;
    } catch (const std::string& msg) {
        cerr << msg << endl;
        status = 1;
    } catch (const char* msg) {
        cerr << msg << endl;
        status = 1;
    }
    if (ic) {
        try {
            ic->destroy();
        } catch (const std::string& msg) {
            cerr << msg << endl;
            status = 1;
        }
    }
    return status;
}

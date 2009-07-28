#include <iostream>
#include <IceE/IceE.h>
#include <IceE/Thread.h>
#include <stdint.h>
#include <sys/time.h>
#include <string.h>
#include <cmath>
#include "scope.h"

using namespace std;

class OscilloscopeImpl : virtual public ram::sonar::scope::Oscilloscope, virtual public IceUtil::Thread {
private:
    bool hasViewer;
    ram::sonar::scope::ViewerPrx viewerPrx;
    
    volatile int iHoldoff;
    volatile int iSkip;
    bool isCapturing;
    
    volatile short horizontalZoom;
    volatile short triggerChannel;
    volatile short triggerLevel;
    volatile int triggerHoldoff;
    volatile ::ram::sonar::scope::TriggerMode triggerMode;
    volatile ::ram::sonar::scope::TriggerSlope triggerSlope;
    
    ::ram::sonar::scope::OscilloscopeCapture lastCapture;
    
    
    static suseconds_t getMicroseconds()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_usec;
    }
    
    bool isSampleAvailable()
    {
        static suseconds_t lastTriggered = getMicroseconds();
        
        suseconds_t now = getMicroseconds();
        if (uint8_t(now - lastTriggered) > 100)
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
            val = sin(2 * M_PI * sampleCount / 25) * 10000;
        else
            val = 0;
        
        for (int i = 0 ; i < 4 ; i ++)
            sample[i] = val;
    }
    
    
    
    uint64_t iSample;
    uint16_t iBuf;
    static const unsigned int BUFSIZE = 800;
    int16_t lastSample[4];
    int16_t buf[BUFSIZE][4];
public:
    
    OscilloscopeImpl() :
    hasViewer(false), horizontalZoom(0),
    iHoldoff(0), iSkip(0), iBuf(0), isCapturing(false), iSample(0),
    triggerChannel(0), triggerLevel(0), triggerHoldoff(0),
    triggerMode(::ram::sonar::scope::TriggerModeAuto),
    triggerSlope(::ram::sonar::scope::TriggerSlopeRising)
    {
        lastCapture.rawData = ::ram::sonar::scope::ShortSeq(BUFSIZE * 4);
        lastCapture.timestamp = 0;
    }
    
    virtual void SetViewer(const ::ram::sonar::scope::ViewerPrx& viewerPrx, const ::Ice::Current&)
    {
        cerr << "SetViewer: " << viewerPrx << endl;
        this->viewerPrx = viewerPrx;
        hasViewer = true;
    }
    
    virtual void SetTriggerMode(::ram::sonar::scope::TriggerMode triggerMode, const ::Ice::Current&)
    {
        cerr << "SetTriggerMode: ";
        switch (triggerMode)
        {
            case ::ram::sonar::scope::TriggerModeStop:
                cerr << "Stop";
                break;
            case ::ram::sonar::scope::TriggerModeRun:
                cerr << "Run";
                break;
            case ::ram::sonar::scope::TriggerModeAuto:
                cerr << "Auto";
                break;
        }
        cerr << endl;
        this->triggerMode = triggerMode;
    }
    
    virtual void SetTriggerChannel(::Ice::Short triggerChannel, const ::Ice::Current&)
    {
        cerr << "SetTriggerChannel: " << triggerChannel << endl;
        this->triggerChannel = triggerChannel;
    }
    
    virtual void SetTriggerLevel(::Ice::Short triggerLevel, const ::Ice::Current&)
    {
        cerr << "SetTriggerLevel: " << triggerLevel << endl;
        this->triggerLevel = triggerLevel;
    }
    
    virtual void SetTriggerHoldoff(::Ice::Int triggerHoldoff, const ::Ice::Current&)
    {
        cerr << "SetTriggerHoldoff: " << triggerHoldoff << endl;
        this->triggerHoldoff = triggerHoldoff;
    }
    
    virtual void SetTriggerSlope(::ram::sonar::scope::TriggerSlope triggerSlope, const ::Ice::Current&)
    {
        cerr << "SetTriggerSlope: ";
        switch (triggerSlope)
        {
            case ram::sonar::scope::TriggerSlopeRising:
                cerr << "Rising";
                break;
            case ram::sonar::scope::TriggerSlopeFalling:
                cerr << "Falling";
                break;
        }
        cerr << endl;
        this->triggerSlope = triggerSlope;
    }
    
    /*
    virtual void SetHorizontalPosition(::Ice::Int, const ::Ice::Current&)
    {
    }
    */
    
    virtual void SetHorizontalZoom(::Ice::Short zoom, const ::Ice::Current&)
    {
        cerr << "SetHorizontalZoom: " << zoom << endl;
        horizontalZoom = zoom;
    }
    
    virtual ::ram::sonar::scope::OscilloscopeCapture GetLastCapture(const ::Ice::Current&)
    {
        cerr << "GetLastCapture" << endl;
        return lastCapture;
    }
    
    void start()
    {
    }
    
    void run()
    {
        cerr << "run" << endl;
        
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
            
            ++iSample;
            
            ++iSkip;
            
            int16_t lastValueForTrigger = lastSample[triggerChannel];
            memcpy(lastSample, sample, sizeof(sample));
            
            if (iSkip >= (1 << horizontalZoom))
            {
                // use this sample and reset the skip counter
                iSkip = 0;
                
                if (iHoldoff == 0 && !isCapturing && triggerMode != ::ram::sonar::scope::TriggerModeStop)
                {
                    int16_t thisValueForTrigger = sample[triggerChannel];
                    
                    int16_t trigLev = triggerLevel;
                    if (
                        (triggerSlope == ::ram::sonar::scope::TriggerSlopeRising &&
                         lastValueForTrigger < trigLev &&
                         thisValueForTrigger >= trigLev) ||
                        (lastValueForTrigger > trigLev &&
                         thisValueForTrigger <= trigLev)
                        )
                    {
                        isCapturing = true;
                        iBuf = 0;
                    }
                }
                
                if (isCapturing)
                {
                    memcpy(buf[iBuf++], sample, sizeof(sample));
                    if (iBuf >= BUFSIZE)
                    {
                        isCapturing = false;
                        
                        if (triggerMode == ::ram::sonar::scope::TriggerModeRun)
                            triggerMode = ::ram::sonar::scope::TriggerModeStop;
                        
                        copy(*buf, buf[BUFSIZE], lastCapture.rawData.begin());
                        lastCapture.timestamp = (iSample << 1) / 1000;
                        
                        iHoldoff = triggerHoldoff;
                        
                        cerr << "Acquired" << endl;
                        if (hasViewer)
                        {
                            try {
                                viewerPrx->NotifyCapture();
                            } catch (const Ice::Exception& ex) {
                                cerr << "Exception while calling NotifyCapture:" << endl;
                                cerr << ex << endl;
                            }
                        }
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

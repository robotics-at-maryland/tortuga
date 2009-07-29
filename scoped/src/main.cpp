#ifdef __BFIN
#include "addresses.h"
#endif

#include "fft.h"

#include <stdlib.h>
#include <sys/time.h>
#include <cmath>
#include <iostream>
#include <IceE/IceE.h>
#include <IceE/Thread.h>
#include <IceE/Mutex.h>
#include <stdint.h>
#include <string.h>
#include "scope.h"

using namespace std;
using namespace ram::sonar;

class OscilloscopeImpl : virtual public scope::Oscilloscope, virtual public IceUtil::Thread {
private:
    scope::ViewerPrx viewerPrx;
    scope::ScopeMode scopeMode;
    
    volatile int iHoldoff;
    volatile int iSkip;
    bool isCapturing;
    
    volatile short horizontalZoom;
    volatile short triggerChannel;
    volatile short triggerLevel;
    volatile int triggerHoldoff;
    volatile scope::TriggerMode triggerMode;
    volatile scope::TriggerSlope triggerSlope;
    
    scope::OscilloscopeCapture lastCapture;
    IceUtil::Mutex lastCaptureMutex;
    
#if !defined(__BFIN) || defined(SIMULATE_SIGNAL)
    static suseconds_t getMicroseconds()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_usec;
    }
#endif
    
    void initADCs()
    {
#ifdef __BFIN
        // Initialize ADC
        REG(ADDR_ADCONFIG) = 0x8000;
        REG(ADDR_ADPRESCALER) = 0x0000;
        REG(ADDR_ADCONFIG) = 0x4044;
#endif
    }
    
    bool isSampleAvailable()
    {
#if defined(__BFIN) && !defined(SIMULATE_SIGNAL)
        return REG(ADDR_FIFO_COUNT1A) >= 2;
#else
        static suseconds_t lastTriggered = getMicroseconds();
        
        suseconds_t now = getMicroseconds();
        if (uint8_t(now - lastTriggered) > 100)
        {
            lastTriggered = now;
            return true;
        } else
            return false;
#endif
    }
    
    void getSample(int16_t sample[4])
    {
#if defined(__BFIN) && !defined(SIMULATE_SIGNAL)
        sample[0] = REG(ADDR_FIFO_OUT1A);
        sample[1] = REG(ADDR_FIFO_OUT1B);
        sample[2] = REG(ADDR_FIFO_OUT2A);
        sample[3] = REG(ADDR_FIFO_OUT2B);
#else
        static uint64_t sampleCount = 0;
        ++sampleCount;
        
        int16_t val;
        if ((sampleCount / 1000) % 5 == 0)
            val = (int16_t)(sin(2 * M_PI * sampleCount / ((sampleCount / 5000) % 40)) * sin(M_PI * (sampleCount % 1000) / 1000) * 200);
        else
            val = 0;
        
        for (int i = 0 ; i < 4 ; i ++)
            sample[i] = (int16_t)(val + (((double)rand()/RAND_MAX)*2-1)*10);
#endif
    }
    
    
    
    uint64_t iSample;
    uint16_t iBuf;
    static const unsigned int BUFSIZE = 800;
    int16_t lastSample[4];
    int16_t buf[BUFSIZE][4];
    
    
    int iDecay;
    static const unsigned FFTSIZE = 512;
    int16_t fftIn[FFTSIZE];
    fft_fract16<FFTSIZE> fft;
    scope::SpectrumCapture lastSpectrum;
    
public:
    
    OscilloscopeImpl() :
    scopeMode(scope::ScopeModeOscilloscope),
    horizontalZoom(0),
    iHoldoff(0), iSkip(0), iBuf(0), isCapturing(false), iSample(0),
    triggerChannel(0), triggerLevel(0), triggerHoldoff(1000),
    triggerMode(scope::TriggerModeStop),
    triggerSlope(scope::TriggerSlopeRising)
    {
        lastCapture.rawData = scope::ShortSeq(BUFSIZE * 4);
        lastCapture.timestamp = 0;
        
        lastSpectrum.currentLevels = scope::IntSeq(FFTSIZE * 4);
        lastSpectrum.peakLevels = scope::IntSeq(FFTSIZE * 4);
        lastSpectrum.timestamp = 0;
    }
    
    virtual void SetViewer(const scope::ViewerPrx& viewerPrx, const ::Ice::Current&)
    {
#ifndef NDEBUG
        cerr << "SetViewer: " << viewerPrx << endl;
#endif
        this->viewerPrx = viewerPrx;
    }
    
    virtual void SetScopeMode(scope::ScopeMode scopeMode, const ::Ice::Current&)
    {
#ifndef NDEBUG
        cerr << "SetScopeMode: ";
        switch (scopeMode)
        {
            case scope::ScopeModeOscilloscope:
                cerr << "Oscilloscope";
                break;
            case scope::ScopeModeSpectrumAnalyzer:
                cerr << "SpectrumAnalyzer";
                break;
        }
        cerr << endl;
#endif
        this->scopeMode = scopeMode;
    }
    
    virtual void SetTriggerMode(scope::TriggerMode triggerMode, const ::Ice::Current&)
    {
#ifndef NDEBUG
        cerr << "SetTriggerMode: ";
        switch (triggerMode)
        {
            case scope::TriggerModeStop:
                cerr << "Stop";
                break;
            case scope::TriggerModeRun:
                cerr << "Run";
                break;
            case scope::TriggerModeAuto:
                cerr << "Auto";
                break;
        }
        cerr << endl;
#endif
        this->triggerMode = triggerMode;
    }
    
    virtual void SetTriggerChannel(::Ice::Short triggerChannel, const ::Ice::Current&)
    {
#ifndef NDEBUG
        cerr << "SetTriggerChannel: " << triggerChannel << endl;
#endif
        this->triggerChannel = triggerChannel;
    }
    
    virtual void SetTriggerLevel(::Ice::Short triggerLevel, const ::Ice::Current&)
    {
#ifndef NDEBUG
        cerr << "SetTriggerLevel: " << triggerLevel << endl;
#endif
        this->triggerLevel = triggerLevel;
    }
    
    virtual void SetTriggerHoldoff(::Ice::Int triggerHoldoff, const ::Ice::Current&)
    {
#ifndef NDEBUG
        cerr << "SetTriggerHoldoff: " << triggerHoldoff << endl;
#endif
        this->triggerHoldoff = triggerHoldoff;
    }
    
    virtual void SetTriggerSlope(scope::TriggerSlope triggerSlope, const ::Ice::Current&)
    {
#ifndef NDEBUG
        cerr << "SetTriggerSlope: ";
        switch (triggerSlope)
        {
            case scope::TriggerSlopeRising:
                cerr << "Rising";
                break;
            case scope::TriggerSlopeFalling:
                cerr << "Falling";
                break;
        }
        cerr << endl;
#endif
        this->triggerSlope = triggerSlope;
    }
    
    /*
    virtual void SetHorizontalPosition(::Ice::Int, const ::Ice::Current&)
    {
    }
    */
    
    virtual void SetHorizontalZoom(::Ice::Short zoom, const ::Ice::Current&)
    {
#ifndef NDEBUG
        cerr << "SetHorizontalZoom: " << zoom << endl;
#endif
        horizontalZoom = zoom;
    }
    
    virtual scope::OscilloscopeCapture GetLastCapture(const ::Ice::Current&)
    {
        IceUtil::Mutex::Lock lock(lastCaptureMutex);
#ifndef NDEBUG
        cerr << "GetLastCapture" << endl;
#endif
        return lastCapture;
    }
    
    virtual scope::SpectrumCapture GetLastSpectrum(const ::Ice::Current&)
    {
#ifndef NDEBUG
        cerr << "GetLastSpectrum" << endl;
#endif
        return lastSpectrum;
    }
    
protected:
    virtual void run()
    {
#ifndef NDEBUG
        cerr << "run" << endl;
#endif
        initADCs();
        
        while (true)
        {
            
            iBuf = iHoldoff = iSkip = isCapturing = 0;
            
            while (scopeMode == scope::ScopeModeOscilloscope)
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
                    
                    if (iHoldoff == 0 && !isCapturing && triggerMode != scope::TriggerModeStop)
                    {
                        int16_t thisValueForTrigger = sample[triggerChannel];
                        
                        int16_t trigLev = triggerLevel;
                        if (
                            (triggerSlope == scope::TriggerSlopeRising &&
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
                            
                            if (triggerMode == scope::TriggerModeRun)
                                triggerMode = scope::TriggerModeStop;
                            
                            {
                                IceUtil::Mutex::Lock lock(lastCaptureMutex);
                                copy(*buf, buf[BUFSIZE], lastCapture.rawData.begin());
                                lastCapture.timestamp = (iSample << 1) / 1000;
                                lastCapture.newTriggerMode = triggerMode;
                            }
                            
                            iHoldoff = triggerHoldoff;
                            
#ifndef NDEBUG
                            cerr << "Acquired" << endl;
#endif
                            try {
#ifndef NDEBUG
                                cerr << "Pre-NotifyCapture" << endl;
#endif
                                viewerPrx->NotifyCapture();
#ifndef NDEBUG
                                cerr << "Post-NotifyCapture" << endl;
#endif
                            } catch (const Ice::Exception& ex) {
                                cerr << "Exception while calling NotifyCapture:" << endl;
                                cerr << ex << endl;
                            }
                        }
                    }
                }
            }
            
            
            iBuf = 0;
            iDecay = 0;
            
            while (scopeMode == scope::ScopeModeSpectrumAnalyzer)
            {
                // Busy wait until a sample is available
                while (!isSampleAvailable())
                    ;
                
                // Read new sample
                getSample(buf[iBuf++]);
                
                if (iBuf == FFTSIZE)
                {
                    iBuf = 0;
                    
                    if (iDecay++ == 10)
                    {
                        iDecay = 0;
                        
                        // Every ten windows, decay the peaks
                        for (int i = 0 ; i < FFTSIZE ; i ++)
                        {
                            for (int channel = 0 ; channel < 4 ; channel ++)
                            {
                                int32_t& peakLevel = lastSpectrum.peakLevels[i * 4 + channel];
                                peakLevel = ((uint64_t)peakLevel << 11) / ((1 << 11) + 1);
                            }
                        }
                    }
                    
                    for (int channel = 0 ; channel < 4 ; channel ++)
                    {
                        for (int i = 0 ; i < FFTSIZE ; i ++)
                            fftIn[i] = buf[i][channel];
                        
                        fft.doDFT(fftIn);
                        
                        for (int i = 0 ; i < FFTSIZE ; i ++)
                        {
                            complex_fract16& amp = fft.dftOut[i];
                            
                            int32_t& curLevel = lastSpectrum.currentLevels[i * 4 + channel];
                            int32_t& peakLevel = lastSpectrum.peakLevels[i * 4 + channel];
                            
                            curLevel = 
                            (uint32_t)((int32_t)amp.re * amp.re) + 
                            (uint32_t)((int32_t)amp.im * amp.im);
                            
                            if (curLevel > peakLevel)
                                peakLevel = curLevel;
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
    
    Ice::CommunicatorPtr ic = Ice::initialize(argc, argv);
    
    try {
        
        Ice::ObjectAdapterPtr adapter = ic->createObjectAdapterWithEndpoints(
            "scoped", "default -p 10000");
        
        IceUtil::Handle<OscilloscopeImpl> osc = new OscilloscopeImpl;
        adapter->add(osc, ic->stringToIdentity("osc"));
        adapter->activate();
        osc->start();
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

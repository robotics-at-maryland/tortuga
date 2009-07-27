#include <iostream>
#include <IceE/IceE.h>
#include "scope.h"

using namespace std;

class OscilloscopeImpl : virtual public ram::sonar::scope::Oscilloscope {
public:
    virtual void SetViewer(const ::ram::sonar::scope::ViewerPrx&, const ::Ice::Current&)
    {
    }
    
    virtual void SetTriggerMode(::ram::sonar::scope::TriggerMode, const ::Ice::Current&)
    {
    }
    
    virtual void SetTriggerChannel(::Ice::Short, const ::Ice::Current&)
    {
    }
    
    virtual void SetTriggerLevel(::Ice::Short, const ::Ice::Current&)
    {
    }
    
    virtual void SetTriggerHoldoff(::Ice::Int, const ::Ice::Current&)
    {
    }
    
    virtual void SetHorizontalPosition(::Ice::Int, const ::Ice::Current&)
    {
    }
    
    virtual void SetHorizontalZoom(::Ice::Short, const ::Ice::Current&)
    {
    }
    
    virtual ::ram::sonar::scope::OscilloscopeCapture GetLastCapture(const ::Ice::Current&)
    {
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

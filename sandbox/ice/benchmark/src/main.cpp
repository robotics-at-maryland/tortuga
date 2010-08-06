#include <iostream>
#include <iomanip>
#include <Ice/Ice.h>
#include <sys/time.h>
#include "bench.h"

using namespace std;

class FooServant : public virtual Bench::Foo {
public:
    FooServant() {}
    ~FooServant() {}
    
    virtual Ice::Int bar(Ice::Int i, const Ice::Current& c = Ice::Current())
    {
        return i * i;
    }
    virtual Ice::Int bat(Ice::Int i, const Ice::Current& c = Ice::Current())
    {
        return i * i;
    }
};

static const int nTrials = 1000000;

double secondsForHandleInvoke()
{
    Bench::FooPtr foo = new FooServant();
    
    timeval start, end;
    gettimeofday(&start, NULL);
    for (int i = 0 ; i < nTrials ; i ++)
        int result = foo->bar(0);
    gettimeofday(&end, NULL);
    
    return (end.tv_sec - start.tv_sec) + 0.000001 * (end.tv_usec - start.tv_usec);
}

double secondsForDirectInvoke()
{
    FooServant foo;
    
    timeval start, end;
    gettimeofday(&start, NULL);
    for (int i = 0 ; i < nTrials ; i ++)
        int result = foo.bar(0);
    gettimeofday(&end, NULL);
    
    return (end.tv_sec - start.tv_sec) + 0.000001 * (end.tv_usec - start.tv_usec);
}

double secondsForProxyInvoke()
{
    int status = 0;
    double time = 0;
    Ice::CommunicatorPtr ic;
    try {
        
        // Start up ICE
        ic = Ice::initialize();
        Ice::ObjectAdapterPtr adapter
		= ic->createObjectAdapterWithEndpoints("FooAdapter", "default -p 10000");
        Bench::FooPtr object = new FooServant;
        adapter->add(object, ic->stringToIdentity("foo1"));
        adapter->activate();
        
        Bench::FooPrx fooPrx = Bench::FooPrx::uncheckedCast(
            ic->stringToProxy("foo1:default -p 10000"));
        
        timeval start, end;
        gettimeofday(&start, NULL);
        for (int i = 0 ; i < nTrials ; i ++)
            int result = fooPrx->bar(0);
        gettimeofday(&end, NULL);
        time = (end.tv_sec - start.tv_sec) + 0.000001 * (end.tv_usec - start.tv_usec);
    } catch (const Ice::Exception& e) {
        cerr << e << endl;
        status = 1;
    } catch (const char* msg) {
        cerr << msg << endl;
        status = 1;
    }
    if (ic) {
        try {
            ic->destroy();
        } catch (const Ice::Exception& e) {
            cerr << e << endl;
            status = 1;
        }
    }
    if (status != 0)
    {
        cerr << "Terminating test." << endl;
        exit(status);
    }
    
    return time;
}

double secondsForProxyIdempotentInvoke()
{
    int status = 0;
    double time = 0;
    Ice::CommunicatorPtr ic;
    try {
        
        // Start up ICE
        ic = Ice::initialize();
        Ice::ObjectAdapterPtr adapter
		= ic->createObjectAdapterWithEndpoints("FooAdapter", "default -p 10000");
        Bench::FooPtr object = new FooServant;
        adapter->add(object, ic->stringToIdentity("foo1"));
        adapter->activate();
        
        Bench::FooPrx fooPrx = Bench::FooPrx::uncheckedCast(
            ic->stringToProxy("foo1:default -p 10000"));
        
        timeval start, end;
        gettimeofday(&start, NULL);
        for (int i = 0 ; i < nTrials ; i ++)
            int result = fooPrx->bat(0);
        gettimeofday(&end, NULL);
        time = (end.tv_sec - start.tv_sec) + 0.000001 * (end.tv_usec - start.tv_usec);
    } catch (const Ice::Exception& e) {
        cerr << e << endl;
        status = 1;
    } catch (const char* msg) {
        cerr << msg << endl;
        status = 1;
    }
    if (ic) {
        try {
            ic->destroy();
        } catch (const Ice::Exception& e) {
            cerr << e << endl;
            status = 1;
        }
    }
    if (status != 0)
    {
        cerr << "Terminating test." << endl;
        exit(status);
    }
    
    return time;
}

int
main(int argc, char* argv[])
{
    cout << "Direct invoke: ";
    double directInvoke = secondsForDirectInvoke();
    cout << setw(8) << directInvoke << setw(8) << directInvoke / directInvoke << endl;
    
    cout << "Handle invoke: ";
    double handleInvoke = secondsForHandleInvoke();
    cout << setw(8) << handleInvoke << setw(8) << handleInvoke / directInvoke << endl;
    
    cout << "Proxy invoke:  ";
    double proxyInvoke = secondsForProxyInvoke();
    cout << setw(8) << proxyInvoke << setw(8) << proxyInvoke / directInvoke << endl;
    
    cout << "Idemp. invoke: ";
    double idempInvoke = secondsForProxyIdempotentInvoke();
    cout << setw(8) << idempInvoke << setw(8) << idempInvoke / directInvoke << endl;
    
    return 0;
}

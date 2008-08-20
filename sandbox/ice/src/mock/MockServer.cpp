#include <Ice/Ice.h>
#include "MockVehicleFactory.h"

using namespace std;

int
main(int argc, char* argv[])
{
    int status = 0;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter
		= ic->createObjectAdapterWithEndpoints("MockAdapter", "default -p 10000");
        Ice::ObjectPtr object = new ram::mock::MockVehicleFactory;
        adapter->add(object, ic->stringToIdentity("factory"));
        adapter->activate();
        ic->waitForShutdown();
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
    return status;
}

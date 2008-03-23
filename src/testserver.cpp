#include <Ice/Ice.h>
#include <control/MockController.h>

using namespace std;

int
main(int argc, char* argv[])
{
    int status = 0;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter
		= ic->createObjectAdapterWithEndpoints(
											   "MockControllerAdapter", "default -p 10000");
        Ice::ObjectPtr object = new MockController;
        adapter->add(object, ic->stringToIdentity("MockController"));
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
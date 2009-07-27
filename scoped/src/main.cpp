#include <iostream>
#include <IceE/IceE.h>
#include "scope.h"

using namespace std;

int main(int argc, char* argv[])
{
    int status = 0;
    Ice::CommunicatorPtr ic;
    
    try {
        
        ic = Ice::initialize(argc, argv);
        
        shutdownOnInterrupt();
        
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

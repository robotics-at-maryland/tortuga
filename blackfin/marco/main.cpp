/*
 *  main.cpp
 *  
 *
 *  Created by auvsi on 4/2/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <IceE/IceE.h>
#include <iostream>
#include <marcopolo.h>
#include "MarcoI.h"


using namespace std;

int main(int argc, char* argv[])
{
	int status = 0;
    Ice::CommunicatorPtr ic;
	
	try {
		ic = Ice::initialize(argc, argv);
		Ice::ObjectAdapterPtr adapter
		= ic->createObjectAdapterWithEndpoints("MarcoAdapter", "default -p 10000");
		
		Ice::ObjectPtr object = new MarcoI;
		adapter->add(object, ic->stringToIdentity("Marco"));
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

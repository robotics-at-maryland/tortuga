#include "common.hpp"

#include <sys/time.h>
#include <stdlib.h>

class Server : public ClientOrServer {
public:
	
	timeval start;
	
	ram::events::SonarEvent randomPing()
	{
		timeval now;
		gettimeofday(&now, NULL);
		ram::events::SonarEvent evt;
		evt.range = (double)rand() / RAND_MAX * 100.0;
		evt.pingTimeSec = now.tv_sec - start.tv_sec;
		evt.pingTimeUSec = now.tv_usec - start.tv_usec;
		return evt;
	}
		
	virtual int run(int argc, char* argv[])
	{
		ClientOrServer::run(argc, argv);
		gettimeofday(&start, NULL);
		
		// Get a proxy to the publisher for this event
		ram::events::SonarEventListenerPrx publisher =
		ram::events::SonarEventListenerPrx::uncheckedCast(sonarTopic->getPublisher());
		
		while (true)
		{
			cerr << "Ping!" << endl;
			publisher->pingReceived(randomPing());
			
			// wait 1 second
			IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
		}
		
		communicator()->waitForShutdown();
	}
};

int main(int argc, char* argv[])
{
	Server server;
	return server.main(argc, argv, "config.pub");
}

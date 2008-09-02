#include "common.hpp"

#include <sys/time.h>
#include <stdlib.h>

class Server : public ClientOrServer {
public:
	virtual int run(int argc, char* argv[])
	{
		ClientOrServer::run(argc, argv);
		
		ram::events::SonarEventListenerPrx sonarListener =
		ram::events::SonarEventListenerPrx::uncheckedCast(sonarTopic->getPublisher());
		
		timeval start;
		gettimeofday(&start, NULL);
		while (true)
		{
			timeval now;
			gettimeofday(&now, NULL);
			ram::events::SonarEvent evt;
			evt.range = (double)rand() / RAND_MAX * 100.0;
			evt.pingTimeSec = now.tv_sec - start.tv_sec;
			evt.pingTimeUSec = now.tv_usec - start.tv_usec;
			cerr << "Ping!" << endl;
			sonarListener->pingReceived(evt);
			
			// Keep going until the program is killed
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

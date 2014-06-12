#include "common.hpp"

class SonarEventPrinter : public ram::events::SonarEventListener {
public:
	virtual void
	pingReceived(const ram::events::SonarEvent& evt, const Ice::Current&)
	{
		cout << "Ping received at " << evt.pingTimeSec + evt.pingTimeUSec * 0.000001
			 << " seconds with a range of " << evt.range << " meters." << endl;
	}
};

class Client : public ClientOrServer {
public:
	virtual int run(int argc, char* argv[])
	{
		ClientOrServer::run(argc, argv);
		
		// Register a new object adapter
		Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapter("SonarEventListener.Subscriber");
		
		// Subscribe to the event
		IceStorm::QoS qos;
		Ice::ObjectPrx subscriber = adapter->addWithUUID(new SonarEventPrinter);
		sonarTopic->subscribe(qos, subscriber);
		
		// Start the adapter and keep going until the program is killed
		adapter->activate();
		shutdownOnInterrupt();
		communicator()->waitForShutdown();
		sonarTopic->unsubscribe(subscriber);
		return EXIT_SUCCESS;
	}
};

int main(int argc, char* argv[])
{
	Client client;
	return client.main(argc, argv, "config.sub");
}

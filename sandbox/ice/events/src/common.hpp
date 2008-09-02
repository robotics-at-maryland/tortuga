#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// ICE-generated header file
#include <events.h>

using namespace std;

IceStorm::TopicPrx createOrRetrieveTopic(IceStorm::TopicManagerPrx manager, const std::string& topicName)
{
	try {
		return manager->retrieve(topicName);
	} catch (const IceStorm::NoSuchTopic&) {
		return manager->create(topicName);
	}
}

class ClientOrServer : public Ice::Application {
public:
	IceStorm::TopicPrx sonarTopic;
	
	virtual int run(int argc, char* argv[])
	{
		// Connect to the topic manager
		IceStorm::TopicManagerPrx topicManager = IceStorm::TopicManagerPrx::checkedCast(
			communicator()->propertyToProxy("TopicManager.Proxy"));
		sonarTopic = createOrRetrieveTopic(topicManager, "Sonar");
	}
};

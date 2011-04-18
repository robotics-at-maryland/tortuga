
#include <wx/wx.h>

#include "core/include/EventHub.h"
#include "network/include/NetworkHub.h"

using namespace ram;

class MainApp : public wxApp
{
public:
    virtual bool OnInit();

private:
    core::EventHubPtr eventHub;
};

bool MainApp::OnInit()
{
    /* start the network hub */
    /* note: this is currently using the default constructor */
    eventHub = core::EventHubPtr(new network::NetworkHub());

    /* initialize wx frame */
    // TODO

    return false;
}

IMPLEMENT_APP(MainApp)

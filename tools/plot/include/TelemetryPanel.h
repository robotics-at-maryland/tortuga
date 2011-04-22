/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  tools/plot/include/Telemetrypanel.h
 */

#ifndef RAM_TOOLS_TELEMENTRYPANEL
#define RAM_TOOLS_TELEMENTRYPANEL

#include "wx/wx.h"
#include "core/include/EventHub.h"

using namespace ram;

class TelemetryPanel : public wxPanel
{
public:
    TelemetryPanel(wxWindow* parent,
                   core::EventHubPtr eventHub,
                   wxString name = wxT("default_name"));

    ~TelemetryPanel() {}

private:

    core::EventHubPtr m_eventHub;
    std::vector< core::EventConnectionPtr > m_connections;

};


#endif // RAM_TOOLS_TELEMETRYPANEL

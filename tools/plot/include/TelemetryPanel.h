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

#include <map>
#include "wx/wx.h"
#include "wx/grid.h"
#include "wx/aui/aui.h"
#include "core/include/EventHub.h"
#include "core/include/Event.h"
#include "core/include/EventConnection.h"
#include "RotationCtrl.h"
#include "math/include/Quaternion.h"
#include "math/include/AveragingFilter.h"

using namespace ram;

class TelemetryPanel : public wxPanel
{
public:
    TelemetryPanel(wxWindow* parent, core::EventHubPtr eventHub);

    ~TelemetryPanel() {}

    wxAuiPaneInfo& info();

private:
    core::EventHubPtr m_eventHub;
    std::vector< core::EventConnectionPtr > m_connections;
    wxAuiPaneInfo m_info;
};


class OrientationTelemetryPanel : public wxPanel
{
public:
    OrientationTelemetryPanel(wxWindow *parent, core::EventHubPtr eventHub);

    ~OrientationTelemetryPanel() {}

    void onOrientationUpdate(core::EventPtr event);

private:
    std::vector< core::EventConnectionPtr > m_connections;

    RotationCtrl *m_roll;
    RotationCtrl *m_pitch;
    RotationCtrl *m_yaw;

    math::Quaternion m_desOrientation;
    math::Quaternion m_estOrientation;

};

class EventRatePanel : public wxGrid
{
public:
    EventRatePanel(wxWindow *parent, core::EventHubPtr eventHub);
    ~EventRatePanel();

    struct EventData
    {
        wxString name;
        int row;
        math::AveragingFilter<double, 10> filter;
        double timeStamp;
        bool active;
        int timesInactive;
    };
    
    void handler(core::EventPtr event);
    void redraw();

private:
    core::EventHubPtr m_eventHub;
    core::EventConnectionPtr m_connection;

    std::map<core::Event::EventType, EventData> m_eventRateTable;

    time_t m_lastUpdate;
    time_t m_updatePeriod;
};

#endif // RAM_TOOLS_TELEMETRYPANEL

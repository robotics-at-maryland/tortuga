/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  tools/plot/src/TelemetryPanel.cpp
 */

// STD Includes
#include <ctime>

// Library Includes
#include <boost/bind.hpp>
#include "wx/wx.h"
#include "wx/gbsizer.h"
#include "wx/sizer.h"

// Project Includes
#include "TelemetryPanel.h"
#include "RotationCtrl.h"
#include "estimation/include/IStateEstimator.h"
#include "control/include/IController.h"
#include "math/include/Quaternion.h"
#include "math/include/Events.h"

using namespace ram;

TelemetryPanel::TelemetryPanel(wxWindow* parent, core::EventHubPtr eventHub) :
    wxPanel(parent, wxID_ANY, wxDefaultPosition,
            wxDefaultSize, wxTAB_TRAVERSAL | wxVSCROLL | wxHSCROLL,
            wxT("Telemetry Panel"))
{
    m_info.Gripper(true);
    m_info.GripperTop(true);
    m_info.Movable(true);
    m_info.Resizable(true);
    m_info.Floatable(true);
    m_info.DestroyOnClose(true);
    m_info.CloseButton(false);
    m_info.Show();
    m_info.MinSize(240, 240);
    m_info.BestSize(480, 320);
    m_info.CaptionVisible(false);
    m_info.Center();
    m_info.Layer(0);
    m_info.Caption(wxT("Telemetry Panel"));
    m_info.Name(wxT("Telemetry Panel"));

    SetMinSize(wxSize(240, 240));

    wxSizer *overallBox = new wxBoxSizer(wxVERTICAL);
    wxSizer *topBox = new wxBoxSizer(wxHORIZONTAL);
    wxSizer *orientationPanelBox = new wxBoxSizer(wxHORIZONTAL);

    overallBox->Add(topBox, 2, wxEXPAND | wxALL, 2);
    overallBox->Add(orientationPanelBox, 1, wxEXPAND | wxCENTER | wxALL, 2);

    OrientationTelemetryPanel *orientationPanel = new OrientationTelemetryPanel(this, eventHub);
    orientationPanel->Update();
    orientationPanelBox->Add(orientationPanel, 1, wxEXPAND | wxCENTER | wxALL, 2);

    EventRatePanel *eventRatePanel = new EventRatePanel(this, eventHub);
    topBox->Add(eventRatePanel, 1, wxEXPAND | wxCENTER | wxALL, 2);
    
    math::NumericEventPtr event = math::NumericEventPtr(new math::NumericEvent);

    SetSizer(overallBox);
}

wxAuiPaneInfo& TelemetryPanel::info()
{
    return m_info;
}

            
OrientationTelemetryPanel::OrientationTelemetryPanel(wxWindow *parent,
                                                     core::EventHubPtr eventHub) :
    wxPanel(parent, wxID_ANY, wxDefaultPosition, 
            wxDefaultSize, wxTAB_TRAVERSAL,
            wxT("Orientation Telemetry"))
{
    wxSizer *overallSizer = new wxBoxSizer(wxVERTICAL);
    wxSizer *rotationControls = new wxBoxSizer(wxHORIZONTAL);
    wxSizer *labels = new wxBoxSizer(wxHORIZONTAL);
    
    m_roll = new RotationCtrl(this);
    m_pitch = new RotationCtrl(this, math::Degree(90));
    m_yaw = new RotationCtrl(this);

    m_roll->setOrientation(math::Degree(0), math::Degree(0));
    m_pitch->setOrientation(math::Degree(0), math::Degree(0));
    m_yaw->setOrientation(math::Degree(0), math::Degree(0));

    m_desOrientation = math::Quaternion::IDENTITY;
    m_estOrientation = math::Quaternion::IDENTITY;

    int border = 5;
    rotationControls->Add(m_roll, 2, wxSHAPED | wxALIGN_CENTER | wxALL, border);
    rotationControls->Add(m_pitch, 2, wxSHAPED | wxALIGN_CENTER | wxALL, border);
    rotationControls->Add(m_yaw, 2, wxSHAPED | wxALIGN_CENTER | wxALL, border);

    wxStaticText *rollLabel = new wxStaticText(this, wxID_ANY, wxT("Roll"));
    wxStaticText *pitchLabel = new wxStaticText(this, wxID_ANY, wxT("Pitch"));
    wxStaticText *yawLabel = new wxStaticText(this, wxID_ANY, wxT("Yaw"));
    
    wxColour labelColour = wxColour(wxT("BLACK"));
    rollLabel->SetForegroundColour(labelColour);
    pitchLabel->SetForegroundColour(labelColour);
    yawLabel->SetForegroundColour(labelColour);

    labels->Add(rollLabel, 1, wxSHAPED | wxALIGN_CENTER | wxALL, border);
    labels->Add(pitchLabel, 1, wxSHAPED | wxALIGN_CENTER | wxALL, border);
    labels->Add(yawLabel, 1, wxSHAPED | wxALIGN_CENTER | wxALL, border);


    m_connections.push_back(
        eventHub->subscribeToType(
            estimation::IStateEstimator::ESTIMATED_ORIENTATION_UPDATE,
            boost::bind(&OrientationTelemetryPanel::onOrientationUpdate, this, _1)));
    
    m_connections.push_back(
        eventHub->subscribeToType(
            control::IController::DESIRED_ORIENTATION_UPDATE,
            boost::bind(&OrientationTelemetryPanel::onOrientationUpdate, this, _1)));
    
    overallSizer->Add(labels, 1, wxEXPAND);
    overallSizer->Add(rotationControls, 4, wxEXPAND);
    SetSizer(overallSizer);
}

void OrientationTelemetryPanel::onOrientationUpdate(core::EventPtr event)
{
    math::OrientationEventPtr qevent = 
        boost::dynamic_pointer_cast<math::OrientationEvent>(event);

    math::Quaternion orientation = qevent->orientation;

    if(event->type == estimation::IStateEstimator::ESTIMATED_ORIENTATION_UPDATE)
    {
        m_roll->setOrientation(orientation.getRoll(false), m_desOrientation.getRoll(false));
        m_pitch->setOrientation(orientation.getPitch(false), m_desOrientation.getPitch(false));
        m_yaw->setOrientation(orientation.getYaw(false), m_desOrientation.getYaw(false));

        m_estOrientation = orientation;
    }
    else if(event->type == control::IController::DESIRED_ORIENTATION_UPDATE)
    {
        m_roll->setOrientation(m_estOrientation.getRoll(false), orientation.getRoll(false));
        m_pitch->setOrientation(m_estOrientation.getPitch(false), orientation.getPitch(false));
        m_yaw->setOrientation(m_estOrientation.getYaw(false), orientation.getYaw(false));

        m_desOrientation = orientation;
    }
}


EventRatePanel::EventRatePanel(wxWindow *parent, core::EventHubPtr eventHub) : 
    wxGrid(parent, wxID_ANY),
    m_eventHub(eventHub)
{
    CreateGrid(0,2);
    SetColLabelSize(0);
    SetRowLabelSize(0);

    m_connection =
        eventHub->subscribeToAll(
            boost::bind(&EventRatePanel::handler, this, _1));

    m_lastUpdate = static_cast<time_t>(0.0);
    m_updatePeriod = static_cast<time_t>(1.0/30);
}

EventRatePanel::~EventRatePanel()
{
    if(m_connection->connected())
        m_connection->disconnect();
}


void EventRatePanel::handler(core::EventPtr event)
{
    core::Event::EventType type = event->type;

    if(m_eventRateTable.find(type) != m_eventRateTable.end())
    {
        EventData& data = m_eventRateTable[type];
        
        if(data.active)
            data.filter.addValue(event->timeStamp - data.timeStamp);

        data.timeStamp = event->timeStamp;
        data.active = true;
        data.timesInactive = 0;
    }
    else
    {
        assert(AppendRows() && "Row couldn't be added");
        int row = GetNumberRows() - 1;

        EventData data;
        data.name = wxString(type.c_str(), wxConvUTF8);
        data.row = row;
        data.timeStamp = event->timeStamp;
        data.active = true;
        data.timesInactive = 0;

        m_eventRateTable[type] = data;
        SetCellValue(row, 0, data.name);
        SetReadOnly(row, 0, true);
        SetReadOnly(row, 1, true);
    }


    AutoSizeColumn(0);
    AutoSizeColumn(1);

    redraw();
}

void EventRatePanel::redraw()
{
    time_t currTime = std::time(NULL);
    time_t elapsed = std::difftime(currTime, m_lastUpdate);
    elapsed = elapsed;
    if(elapsed > m_updatePeriod)
    {
        std::map<core::Event::EventType, EventData>::iterator iter;
        for(iter = m_eventRateTable.begin(); iter != m_eventRateTable.end(); iter++)
        {
            EventData& data = (*iter).second;
        
            double rate = 0;
            double average = data.filter.getValue();
            if(average != 0)
                rate = 1.0 / average;

            SetCellValue(data.row, 1, wxString::Format(wxT("%f"), rate));
        }

        AutoSizeColumn(0);
        AutoSizeColumn(1);
        m_lastUpdate = currTime;
    }
}

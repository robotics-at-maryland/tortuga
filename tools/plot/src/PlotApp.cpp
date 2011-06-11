/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  tools/plot/src/PlotApp.cpp
 */

#include <iostream>
#include <boost/foreach.hpp>
#include <assert.h>
#include "PlotApp.h"
#include "PlotPanel.h"
#include "TelemetryPanel.h"
#include "core/include/EventHub.h"
#include "network/include/NetworkHub.h"

#include "control/include/IController.h"
#include "estimation/include/IStateEstimator.h"
#include "vehicle/include/device/IDepthSensor.h"

IMPLEMENT_APP(PlotApp)

bool PlotApp::OnInit()
{
    /* start the network hub */
    /* note: this is currently using the default constructor */
    m_eventHub = core::EventHubPtr(new network::NetworkHub());

    PlotFrame *frame = new PlotFrame(wxT("R@M Realtime Plotting"),
                                     wxPoint(0, 0), wxSize(1024, 768));
    
    // initialize the frame
    frame->SetMinSize(wxSize(640,480));

    // Set up the inner windows
    EventSeriesMap depthSeries;
    depthSeries[estimation::IStateEstimator::ESTIMATED_DEPTH_UPDATE] = 
        DataSeriesInfo("Estimated Depth", wxPen(wxColour(wxT("RED")), 5));
    depthSeries[control::IController::DESIRED_DEPTH_UPDATE] = 
        DataSeriesInfo("Desired Depth", wxPen(wxColour(wxT("DARK GREEN")), 5), true);
    depthSeries[vehicle::device::IDepthSensor::UPDATE] = 
        DataSeriesInfo("Measured Depth", wxPen(wxColour(150, 150, 150, 100), 3));

    PlotPanel *depthPanel = new NumericVsTimePlot(
        frame, m_eventHub, depthSeries,
        wxT("Depth Plot"), wxT("Depth Plot"));

   
    EventSeriesMap depthRateSeries;
    depthRateSeries[estimation::IStateEstimator::ESTIMATED_DEPTHRATE_UPDATE] = 
        DataSeriesInfo("Estimated Depth Rate", wxPen(wxColour(wxT("RED")), 5));
    depthRateSeries[control::IController::DESIRED_DEPTHRATE_UPDATE] = 
        DataSeriesInfo("Desired Depth Rate", wxPen(wxColour(wxT("DARK GREEN")), 5), true);

    PlotPanel *depthRatePanel = new NumericVsTimePlot(
        frame, m_eventHub, depthRateSeries,
        wxT("Depth Rate Plot"), wxT("Depth Rate Plot"));


    EventSeriesMap velocitySeries;
    velocitySeries[estimation::IStateEstimator::ESTIMATED_VELOCITY_UPDATE] = 
      DataSeriesInfo("Estimated Velocity", wxPen(wxColour(wxT("RED")), 3));
    velocitySeries[control::IController::DESIRED_VELOCITY_UPDATE] = 
      DataSeriesInfo("Desired Velocity", wxPen(wxColour(wxT("DARK GREEN")), 3));

    PlotPanel *velocityPanel = new Vector2PhasePlot(
        frame, m_eventHub,velocitySeries,
        wxT("Velocity Phase Plot"), wxT("Velocity Phase Plot"));
    velocityPanel->setBufferSize(2000);

    EventSeriesMap positionSeries;
    positionSeries[estimation::IStateEstimator::ESTIMATED_POSITION_UPDATE] =
        DataSeriesInfo("Estimated Position", wxPen(wxColour(wxT("RED")), 3));
    positionSeries[control::IController::DESIRED_POSITION_UPDATE] =
        DataSeriesInfo("Desired Position", wxPen(wxColour(wxT("DARK GREEN")), 3));
    
    PlotPanel *positionPanel = new Vector2PhasePlot(
        frame, m_eventHub, positionSeries,
        wxT("Position Phase Plot"), wxT("Position Phase Plot"));
    positionPanel->setBufferSize(2000);


    depthPanel->info().Show().Left();
    depthRatePanel->info().Show().Left();
    velocityPanel->info().Show().Right();
    positionPanel->info().Show().Right();


    // add the panels
    frame->addPanel(depthPanel, depthPanel->info(), depthPanel->name());
    frame->addPanel(depthRatePanel, depthRatePanel->info(), depthRatePanel->name());
    frame->addPanel(velocityPanel, velocityPanel->info(), velocityPanel->name());
    frame->addPanel(positionPanel, positionPanel->info(), positionPanel->name());

    // show it
    SetTopWindow(frame);
    frame->Show(true);

    return true;
}

PlotFrame::PlotFrame(const wxString& title, const wxPoint& pos,
                     const wxSize& size) :
    wxFrame(0, -1, title, pos, size)
{
    // use wxAUI to manage the window
    m_mgr.SetManagedWindow(this);

    // Set up the menus
    m_menuFile = new wxMenu();
    m_menuFile->Append(ID_Quit, _("E&xit"));

    m_menuPanels = new wxMenu();

    m_menuBar = new wxMenuBar();
    m_menuBar->Append(m_menuFile, _("&File"));
    m_menuBar->Append(m_menuPanels, _("&Panels"));

    SetMenuBar(m_menuBar);

    // update the window manager
    m_mgr.Update();
}

PlotFrame::~PlotFrame()
{
    m_mgr.UnInit();
}

void PlotFrame::onQuit(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.UnInit();
    Close(true);
}

void PlotFrame::addPanel(wxWindow *panel, wxAuiPaneInfo& info, wxString name)
{
    m_panels.push_back(panel);
    m_menuPanels->AppendCheckItem(-1, name);

    int item = m_menuPanels->FindItem(name);
    m_menuPanels->Check(item, info.IsShown());

    m_mgr.AddPane(panel, info);
    m_mgr.Update();
}

void PlotFrame::onCheck(wxCommandEvent& event)
{
    wxMenuItemList& list =  m_menuPanels->GetMenuItems();
    wxMenuItemList::iterator iter;
    for (iter = list.begin(); iter != list.end(); ++iter)
    {
        wxMenuItem *current = *iter;
        wxAuiPaneInfo& paneInfo = m_mgr.GetPane(current->GetName());
        bool isShown = paneInfo.IsShown();
        bool isChecked = current->IsChecked();

        if (isShown && !isChecked)
            paneInfo.Hide();

        if (!isShown && isChecked)
            paneInfo.Show();
    }
    m_mgr.Update();
}

BEGIN_EVENT_TABLE(PlotFrame, wxFrame)
EVT_MENU(ID_Quit,  PlotFrame::onQuit)
EVT_MENU(wxID_ANY, PlotFrame::onCheck)
END_EVENT_TABLE()

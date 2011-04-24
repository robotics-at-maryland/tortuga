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
#include "../include/PlotApp.h"
#include "../include/PlotPanel.h"
#include "../include/TelemetryPanel.h"
#include "core/include/EventHub.h"
#include "network/include/NetworkHub.h"


IMPLEMENT_APP(PlotApp)

bool PlotApp::OnInit()
{
    /* start the network hub */
    /* note: this is currently using the default constructor */
    m_eventHub = core::EventHubPtr(new network::NetworkHub());

    PlotFrame *frame = new PlotFrame( wxT("R@M Realtime Plotting"),
                                      wxPoint(0, 0), wxSize(1024, 768));
    
    // create settings for what the panels can do
    wxAuiPaneInfo defaultInfo = wxAuiPaneInfo();
    defaultInfo.Gripper(true);
    defaultInfo.GripperTop(true);
    defaultInfo.MinSize(240, 180);
    defaultInfo.BestSize(512,320);
    defaultInfo.Movable(true);
    defaultInfo.Resizable(true);
    defaultInfo.CaptionVisible(true);
    defaultInfo.Floatable(true);
    defaultInfo.DestroyOnClose(true);
    defaultInfo.CloseButton(false);

    // initialize the frame
    frame->SetMinSize(wxSize(640,480));

    // Set up the inner windows
    PlotPanel *depthPanel = new DepthPanel(frame, m_eventHub);
    wxAuiPaneInfo depthPanelInfo = wxAuiPaneInfo(defaultInfo);
    wxString depthPanelName = wxT("Depth Plot");
    depthPanelInfo.Caption(depthPanelName);
    depthPanelInfo.Name(depthPanelName);
    depthPanelInfo.Left();
    depthPanelInfo.Layer(2);

    PlotPanel *depthErrorPanel = new DepthErrorPanel(frame, m_eventHub);
    wxAuiPaneInfo depthErrorPanelInfo = wxAuiPaneInfo(defaultInfo);
    wxString depthErrorPanelName = wxT("Depth Error Plot");
    depthErrorPanelInfo.Caption(depthErrorPanelName);
    depthErrorPanelInfo.Name(depthErrorPanelName);
    depthErrorPanelInfo.Bottom();
    depthErrorPanelInfo.Layer(1);

    PlotPanel *depthRatePanel = new DepthRatePanel(frame, m_eventHub);
    wxAuiPaneInfo depthRatePanelInfo = wxAuiPaneInfo(defaultInfo);
    wxString depthRatePanelName = wxT("Depth Rate Plot");
    depthRatePanelInfo.Caption(depthRatePanelName);
    depthRatePanelInfo.Name(depthRatePanelName);
    depthRatePanelInfo.Left();
    depthRatePanelInfo.Layer(2);

    PlotPanel *positionPanel = new VelocityPanel(frame, m_eventHub);
    wxAuiPaneInfo positionPanelInfo = wxAuiPaneInfo(defaultInfo);
    wxString positionPanelName = wxT("Position Plot");
    positionPanelInfo.Caption(positionPanelName);
    positionPanelInfo.Name(positionPanelName);
    positionPanelInfo.Right();
    positionPanelInfo.Layer(2);

    PlotPanel *velocityPanel = new PositionPanel(frame, m_eventHub);
    wxAuiPaneInfo velocityPanelInfo = wxAuiPaneInfo(defaultInfo);
    wxString velocityPanelName = wxT("Velocity Plot");
    velocityPanelInfo.Caption(velocityPanelName);
    velocityPanelInfo.Name(velocityPanelName);
    velocityPanelInfo.Right();
    velocityPanelInfo.Layer(2);

    PlotPanel *testPanel = new TestPanel(frame, m_eventHub);
    wxAuiPaneInfo testPanelInfo = wxAuiPaneInfo(defaultInfo);
    wxString testPanelName = wxT("Test Panel");
    testPanelInfo.Caption(testPanelName);
    testPanelInfo.Name(testPanelName);
    testPanelInfo.Top();
    testPanelInfo.Layer(1);

    TelemetryPanel *telemetryPanel = new TelemetryPanel(frame, m_eventHub);
    wxAuiPaneInfo telemetryPanelInfo = wxAuiPaneInfo(defaultInfo);
    telemetryPanelInfo.MinSize(240, 240);
    telemetryPanelInfo.BestSize(480, 320);
    telemetryPanelInfo.CaptionVisible(false);
    telemetryPanelInfo.Center();
    telemetryPanelInfo.Layer(0);

    // add the panels
    frame->addPanel(telemetryPanel, telemetryPanelInfo, wxString(wxT("Telemetry Panel")));

    frame->addPanel(depthPanel, depthPanelInfo, depthPanelName);
    frame->addPanel(depthRatePanel, depthRatePanelInfo, depthRatePanelName);
    frame->addPanel(positionPanel, positionPanelInfo, positionPanelName);
    frame->addPanel(velocityPanel, velocityPanelInfo, velocityPanelName);

    frame->addPanel(testPanel, testPanelInfo, testPanelName);
    frame->addPanel(depthErrorPanel, depthErrorPanelInfo, depthErrorPanelName);

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
    BOOST_FOREACH(wxWindow* panel, m_panels)
    {
        assert(panel->Destroy() && "Panel Destruction Failed");
    }
    m_mgr.UnInit();

    Close(true);
}

void PlotFrame::addPanel(wxWindow *panel, wxAuiPaneInfo& info, wxString name)
{
    m_panels.push_back(panel);
    m_menuPanels->AppendCheckItem(-1, name);

    int item = m_menuPanels->FindItem(name);
    m_menuPanels->Check(item, true);

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

/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  tools/plot/include/PlotApp.cpp
 */

#ifndef RAM_TOOLS_PLOTAPP
#define RAM_TOOLS_PLOTAPP

#include "wx/wx.h"
#include "wx/laywin.h"
#include "wx/aui/aui.h"

#include "core/include/EventHub.h"
#include "network/include/NetworkHub.h"

using namespace ram;

class PlotApp : public wxApp
{
    virtual bool OnInit();
};

class PlotFrame : public wxFrame
{
public:
    PlotFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    virtual ~PlotFrame();

    void onQuit(wxCommandEvent& event);
    void addPanel(wxWindow *panel, wxAuiPaneInfo& info);

private:
    wxMenuBar *m_menuBar;
    wxMenu *m_menuFile;

    wxAuiManager m_mgr;
public:
    DECLARE_EVENT_TABLE()
};

enum
{
    ID_Quit = 1,
};

BEGIN_EVENT_TABLE(PlotFrame, wxFrame)
    EVT_MENU(ID_Quit,  PlotFrame::onQuit)
END_EVENT_TABLE()

#endif // RAM_TOOLS_PLOTAPP

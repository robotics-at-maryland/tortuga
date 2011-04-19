/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  tools/plot/include/PlotPanel.h
 */

#ifndef RAM_TOOLS_PLOTPANEL
#define RAM_TOOLS_PLOTPANEL

#include "../include/mathplot.h"
#include "wx/wx.h"
#include "core/include/EventHub.h"

using namespace ram;

class PlotPanel : public wxPanel
{
public:
    PlotPanel(wxWindow* parent,
              core::EventHubPtr eventHub,
              wxString name);

    ~PlotPanel() {}

private:
    
    
};


#endif // RAM_TOOLS_PLOTPANEL

/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  tools/plot/src/TelemetryPanel.cpp
 */

#include "../include/TelemetryPanel.h"

TelemetryPanel::TelemetryPanel(wxWindow* parent,
                               core::EventHubPtr eventHub, 
                               wxString name) :
    wxPanel(parent, wxID_ANY, wxDefaultPosition,
            wxDefaultSize, wxTAB_TRAVERSAL, name) 
{

}

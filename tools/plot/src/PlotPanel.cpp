/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  tools/plot/src/PlotPanel.cpp
 */

#include "../include/PlotPanel.h"

PlotPanel::PlotPanel(wxWindow* parent,
                     core::EventHubPtr eventHub, 
                     wxString name) :
    wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL) 
{
    wxSizer *outerBox = new wxBoxSizer(wxVERTICAL);
    wxSizer *plotBox = new wxBoxSizer(wxHORIZONTAL);
    wxSizer *buttonBox = new wxBoxSizer(wxHORIZONTAL);

    outerBox->Add(plotBox, 4, wxEXPAND);
    outerBox->Add(buttonBox, 1, wxEXPAND);

    plotBox->Add(new mpWindow(this, -1), 1, wxEXPAND, 0);

    buttonBox->Add(new wxButton(this, -1, _("Button1")), 1, wxEXPAND, 0);
    buttonBox->Add(new wxButton(this, -1, _("Button2")), 1, wxEXPAND, 0);
    buttonBox->Add(new wxButton(this, -1, _("Button3")), 1, wxEXPAND, 0);

    SetSizer(outerBox);
}

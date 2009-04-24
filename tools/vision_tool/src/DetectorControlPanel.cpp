/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:   tools/vision_viewer/include/DetectorControlPanel.h
 */

// STD Includes
#include <cmath>

// Library Includes
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/event.h>

#include <boost/foreach.hpp>

// Project Includes
#include "DetectorControlPanel.h"
#include "PropertyControl.h"
#include "Model.h"

#include "core/include/PropertySet.h"

namespace ram {
namespace tools {
namespace visionvwr {

BEGIN_EVENT_TABLE(DetectorControlPanel, wxPanel)
END_EVENT_TABLE()
    
DetectorControlPanel::DetectorControlPanel(Model* model,
                                     wxWindow *parent, wxWindowID id,
                                     const wxPoint &pos, const wxSize &size) :
    wxPanel(parent, id, pos, size),
    m_model(model),
    m_choice(0)
{
    wxStaticText* text = new wxStaticText(this, wxID_ANY, wxT("Detector"));
    text->SetWindowStyle(wxALIGN_LEFT);

    // Create our detector choice gui element and add its options
    m_choice = new wxChoice(this, wxID_ANY);
    m_choice->Append(wxT("None"));
    BOOST_FOREACH(std::string name, m_model->getDetectorNames())
    {
        m_choice->Append(wxString(name.c_str(), wxConvUTF8));
    }
    m_choice->SetSelection(0);

    // Create our size and insert the controls
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
    row->Add(text, 0, wxALIGN_LEFT | wxALL, 3);
    row->Add(m_choice, 1, wxALIGN_CENTER | wxALL, 3);

    sizer->Add(row, 0, wxEXPAND, 0);
    sizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxALL, 3);

    sizer->SetSizeHints(this);
    SetSizer(sizer);

    // Connect button event
    Connect(m_choice->GetId(), wxEVT_COMMAND_CHOICE_SELECTED,
            wxCommandEventHandler(DetectorControlPanel::onDetectorChanged));
}
    
DetectorControlPanel::~DetectorControlPanel()
{
}

void DetectorControlPanel::onDetectorChanged(wxCommandEvent& event)
{
    int selectionPos = m_choice->GetSelection();
    std::string selection(m_choice->GetString(selectionPos).mb_str()); 

    if (selection == "None")
        m_model->disableDetector();
    else
        m_model->changeToDetector(selection);

    // Drop all the old properties
    wxSizer *sizer = GetSizer();
    while (sizer->GetChildren().GetCount() > 2)
    {
        wxWindow* win = sizer->GetItem(2)->GetWindow();
        sizer->Remove(2);
        win->Destroy();
    }

    // Now lets get a list of the properties
    core::PropertySetPtr propSet = m_model->getDetectorPropertySet();
    std::vector<std::string> propNames = propSet->getPropertyNames();

    BOOST_FOREACH(std::string name, propSet->getPropertyNames())
    {
        core::PropertyPtr property(propSet->getProperty(name));
        wxSize size(GetSize().GetWidth(), wxDefaultSize.GetHeight());
        sizer->Add(new PropertyControl(property, this, wxID_ANY, 
				       wxDefaultPosition, size), 
		   1, wxEXPAND | wxALL, 3);
    }

    sizer->SetSizeHints(this);

    // Attempt to reset everything
    sizer->Layout();
    SetSize(GetSize());
    Refresh();
    Update();
}
    
} // namespace visionvwr
} // namespace tools
} // namespace ram

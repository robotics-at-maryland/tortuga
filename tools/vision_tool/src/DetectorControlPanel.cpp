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
#include <wx/button.h>

#include <boost/foreach.hpp>
#include <boost/bind.hpp>

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

    // Button that will reset to default values
    wxButton* reset = new wxButton(this, wxID_ANY, wxT("Reset to Defaults"));

    // Create our size and insert the controls
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    
    // Detector Change Controls
    wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
    row->Add(text, 0, wxALIGN_LEFT | wxALL, 3);
    row->Add(m_choice, 1, wxALIGN_CENTER | wxALL, 3);

    // Add All the elements to the main sizer
    sizer->Add(row, 0, wxEXPAND, 0);
    sizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxALL, 3);
    sizer->Add(reset, 0, wxALIGN_CENTER | wxALL, 3);

    sizer->SetSizeHints(this);
    SetSizer(sizer);

    // Subscribe to model events
    m_model->subscribe(Model::DETECTOR_CHANGED,
        boost::bind(&DetectorControlPanel::onDetectorChanged, this, _1));

    // Connect to our events
    Connect(m_choice->GetId(), wxEVT_COMMAND_CHOICE_SELECTED,
            wxCommandEventHandler(DetectorControlPanel::onDetectorSelected));
    Connect(reset->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(DetectorControlPanel::onReset));
    Connect(GetId(), wxEVT_CLOSE_WINDOW,
	    wxCloseEventHandler(DetectorControlPanel::onClose));
}
    
DetectorControlPanel::~DetectorControlPanel()
{
}

void DetectorControlPanel::onDetectorSelected(wxCommandEvent& event)
{
    int selectionPos = m_choice->GetSelection();
    std::string selection(m_choice->GetString(selectionPos).mb_str()); 

    if (selection == "None")
        m_model->disableDetector();
    else
        m_model->changeToDetector(selection);
}

void DetectorControlPanel::onDetectorChanged(core::EventPtr event)
{
    // Drop all the old properties
    wxSizer *sizer = GetSizer();
    BOOST_FOREACH(PropertyControl* propControl, m_propControls)
    {
        sizer->Remove(propControl);
	propControl->Destroy();
    }
    m_propControls.clear();

    // Now lets get a list of the properties
    core::PropertySetPtr propSet = m_model->getDetectorPropertySet();
    if (propSet)
    {
        std::vector<std::string> propNames = propSet->getPropertyNames();
	int insertPos = 2;
        BOOST_FOREACH(std::string name, propSet->getPropertyNames())
        {
            // Create the new property control
            core::PropertyPtr property(propSet->getProperty(name));
            wxSize size(GetSize().GetWidth(), wxDefaultSize.GetHeight());
	    PropertyControl* propControl = 
	      new PropertyControl(property, m_model, this, wxID_ANY, 
				  wxDefaultPosition, size);

	    // Add the new property to our sizer and list
	    sizer->Insert(insertPos, propControl, 1, wxEXPAND | wxALL, 3);
	    m_propControls.push_back(propControl);
	    insertPos++;
        }
    }

    // Attempt to reset everything
    sizer->SetSizeHints(this);
    sizer->Layout();
    SetSize(GetSize());
    Refresh();
    Update();
}

void DetectorControlPanel::onReset(wxCommandEvent& event)
{
    BOOST_FOREACH(PropertyControl* propControl, m_propControls)
    {
	propControl->setToDefault();
    }
}

void DetectorControlPanel::onClose(wxCloseEvent& event)
{
    if (event.CanVeto())
        Hide();
    else
        Destroy();
}
    
} // namespace visionvwr
} // namespace tools
} // namespace ram

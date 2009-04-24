/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:   tools/vision_viewer/include/PropertyControl.h
 */

// STD Includes
#include <cmath>

// Library Includes
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/event.h>

// Project Includes
#include "PropertyControl.h"
#include "Model.h"

#include "core/include/Property.h"

namespace ram {
namespace tools {
namespace visionvwr {

BEGIN_EVENT_TABLE(PropertyControl, wxPanel)
END_EVENT_TABLE()
    
PropertyControl::PropertyControl(core::PropertyPtr property, Model* model,
				 wxWindow *parent, wxWindowID id,
				 const wxPoint &pos, const wxSize &size) :
    wxPanel(parent, id, pos, size),
    m_prop(property),
    m_text(0),
    m_label(0),
    m_model(model),
    m_defaultValue(m_prop->toString().c_str(), wxConvUTF8)
{
    // Label for the property
    wxString propName(m_prop->getName().c_str(), wxConvUTF8);
    m_label = new wxStaticText(this, wxID_ANY, propName);
    m_label->SetWindowStyle(wxALIGN_LEFT);

    // Create the text box
    m_text = new wxTextCtrl(this, wxID_ANY, m_defaultValue, wxDefaultPosition,
			    wxDefaultSize, wxTE_RIGHT | wxTE_PROCESS_ENTER);

    // Create our size and inser the controls
    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(m_label, 0, wxALIGN_LEFT | wxALL, 3);
    sizer->Add(m_text, 1, wxEXPAND | wxALIGN_CENTER | wxALL, 3);
    sizer->SetSizeHints(this);
    SetSizer(sizer);

    // Connect button event
    m_text->Connect(m_text->GetId(), wxEVT_COMMAND_TEXT_UPDATED,
		    wxCommandEventHandler(PropertyControl::onTextUpdated), NULL,
		    this);
    m_text->Connect(m_text->GetId(), wxEVT_COMMAND_TEXT_ENTER,
		    wxCommandEventHandler(PropertyControl::onEnter), NULL,
		    this);
}
    
PropertyControl::~PropertyControl()
{
}

void PropertyControl::setToDefault()
{
    // Reset underlying property
    setPropertyValue(m_defaultValue);
    // Set the text box to reflect that
    m_text->SetValue(m_defaultValue);
}

void PropertyControl::onTextUpdated(wxCommandEvent& event)
{
    wxString propValue(m_prop->toString().c_str(), wxConvUTF8);
    wxString value = event.GetString();

    // Grab the default style and change the colour if we have changed the value
    if (0 == value.CompareTo(propValue))
        m_label->SetBackgroundColour(wxNullColour);
    else
        m_label->SetBackgroundColour(*wxRED);

    event.Skip();
}

void PropertyControl::onEnter(wxCommandEvent& event)
{
    setPropertyValue(m_text->GetValue());
}

void PropertyControl::setPropertyValue(wxString value)
{
    bool converted = false;
    switch (m_prop->getType())
    {
        case core::Property::PT_INT:
	    {
                int val = 0;
		converted = value.ToLong((long int*)&val);
		if (converted)
                    m_prop->set(val);
	    }
            break;
        case core::Property::PT_DOUBLE:
	    {
                double val = 0;
		converted = value.ToDouble(&val);
		if (converted)
                    m_prop->set(val);
	    }
            break;
        default:
	    break;
    }

    if (converted)
    {
        m_label->SetBackgroundColour(wxNullColour);
        m_model->detectorPropertiesChanged();
    }
}
    
} // namespace visionvwr
} // namespace tools
} // namespace ram

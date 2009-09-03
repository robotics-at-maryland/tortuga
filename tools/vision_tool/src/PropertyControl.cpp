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
#include <wx/checkbox.h>
#include <wx/dcclient.h>
#include <wx/slider.h>
#include <wx/button.h>

#include <boost/bind.hpp>

// Project Includes
#include "PropertyControl.h"
#include "Model.h"

#include "core/include/Property.h"
#include "core/include/EventConnection.h"

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
    m_checkBox(0),
    m_slider(0),
    m_sliderScale(1),
    m_label(0),
    m_model(model),
    m_defaultValue(m_prop->toString().c_str(), wxConvUTF8),
    m_defaultButton(0)
{
    // Label for the property
    wxString propName(m_prop->getName().c_str(), wxConvUTF8);
    wxString toolTip(m_prop->getDescription().c_str(), wxConvUTF8);
    m_label = new wxStaticText(this, wxID_ANY, propName);
    m_label->SetWindowStyle(wxALIGN_LEFT);
    m_label->SetToolTip(toolTip);

    // Create our size and insert starting controls
    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(m_label, 0, wxALIGN_LEFT | wxALL, 3);

    // Create the controls for our specific property
    switch (m_prop->getType())
    {
        case core::Property::PT_INT:
        case core::Property::PT_DOUBLE:
            {
		if (m_prop->hasMinMax())
		    setupMinMaxIntDoubleControls(sizer, toolTip);
		else
		    setupNormalIntDoubleControls(sizer, toolTip);
            }
            break;
	    
        case core::Property::PT_BOOL:
	    {
		setupBoolControls(sizer, toolTip);
	    }
	    break;
        
        default:
            break;
    }

    if (m_text)
    {
        m_text->Connect(m_text->GetId(), wxEVT_COMMAND_TEXT_UPDATED,
			wxCommandEventHandler(PropertyControl::onTextUpdated), 
			NULL, this);
	m_text->Connect(m_text->GetId(), wxEVT_COMMAND_TEXT_ENTER,
			wxCommandEventHandler(PropertyControl::onEnter), NULL,
			this);
    }

    m_defaultButton = new wxButton(this, wxID_ANY, wxT("RESET"));
    m_defaultButton->Connect(m_defaultButton->GetId(),
			     wxEVT_COMMAND_BUTTON_CLICKED,
			     wxCommandEventHandler(
			         PropertyControl::onDefaultButton), NULL, this);
    sizer->Add(m_defaultButton);

    sizer->SetSizeHints(this);
    SetSizer(sizer);

    m_propChangeConnection = m_model->subscribe(
        Model:: DETECTOR_PROPERTIES_CHANGED,
        boost::bind(&PropertyControl::onPropertiesChanged, this, _1));
}
    
PropertyControl::~PropertyControl()
{
    // Unsubscribe from the connection
    m_propChangeConnection->disconnect();
}

void PropertyControl::setToDefault()
{
    // Reset underlying property
    setPropertyValue(m_defaultValue);

    // Set the underlying control to reflect that
    switch (m_prop->getType())
    {
        case core::Property::PT_INT:
        case core::Property::PT_DOUBLE:
            m_text->SetValue(m_defaultValue);
            break;

        case core::Property::PT_BOOL:
            m_checkBox->SetValue(m_prop->getAsBool());
            break;
            
        default:
            assert(false && "Error improper property type");
            break;
    }

    
}

wxString PropertyControl::getPropertyValue()
{
    wxString val(m_prop->getName().c_str(), wxConvUTF8);
    val.Append(wxT(": "));
    switch (m_prop->getType())
    {
        case core::Property::PT_INT:
        case core::Property::PT_DOUBLE:
	    val.Append(m_text->GetValue());
	    break;
        case core::Property::PT_BOOL:
	    if (m_checkBox->GetValue())
		val.Append(wxT("1"));
	    else
		val.Append(wxT("0"));
	    break;
        default:
	    assert(false && "Error improper property type");
	    break;
    }
    return val;
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

void PropertyControl::onCheck(wxCommandEvent& event)
{
    m_prop->set(m_checkBox->GetValue());
    m_model->detectorPropertiesChanged();
}

void PropertyControl::onSliderUpdate(wxScrollEvent& event)
{
    switch (m_prop->getType())
    {
        case core::Property::PT_INT:
            m_prop->set((int)event.GetPosition());
            m_slider->SetValue(m_prop->getAsInt());
            break;
        case core::Property::PT_DOUBLE:
            m_prop->set(event.GetPosition() / m_sliderScale);
            m_slider->SetValue((int)(m_prop->getAsDouble() * m_sliderScale));
            break;
        default:
            assert(false && "Error improper type for slider");
            break;
    }

    m_text->ChangeValue(wxString(m_prop->toString().c_str(), wxConvUTF8));
    m_model->detectorPropertiesChanged();
}

void PropertyControl::onPropertiesChanged(core::EventPtr event)
{
    switch (m_prop->getType())
    {
        case core::Property::PT_INT:
            if (m_slider)
                m_slider->SetValue(m_prop->getAsInt());
            m_text->ChangeValue(wxString(m_prop->toString().c_str(), 
					 wxConvUTF8));
            break;
        case core::Property::PT_DOUBLE:
            if (m_slider)
                m_slider->SetValue((int)(m_prop->getAsDouble()*m_sliderScale));
            m_text->ChangeValue(wxString(m_prop->toString().c_str(), 
					 wxConvUTF8));
            break;
        case core::Property::PT_BOOL:
            m_checkBox->SetValue(m_prop->getAsBool());
            break;
        default:
            assert(false && "Error improper property type");
            break;
    }
}

void PropertyControl::onDefaultButton(wxCommandEvent& event)
{
    setToDefault();
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
        case core::Property::PT_BOOL:
            {
                int val = 0;
                converted = value.ToLong((long int*)&val);
                if (converted)
                {
                    m_prop->set((bool)(val == 1));
                }
            }
            break;
        default:
            assert(false && "Error improper property type");
            break;
    }

    if (converted)
    {
        // Change label back the to the normal color
        m_label->SetBackgroundColour(wxNullColour);

        // Read back the property and display it
        if (m_text)
        {
            m_text->ChangeValue(wxString(m_prop->toString().c_str(),
                                         wxConvUTF8));
        }
        else
        {
            m_checkBox->SetValue(m_prop->getAsBool());
        }

        // Notify the model so it updates any processing it has to do
        m_model->detectorPropertiesChanged();
    }
}
  
void PropertyControl::setupNormalIntDoubleControls(wxSizer* sizer,
                                                   wxString toolTip)
{
    m_text = new wxTextCtrl(this, wxID_ANY, m_defaultValue, 
                            wxDefaultPosition, wxDefaultSize, 
                            wxTE_RIGHT | wxTE_PROCESS_ENTER);
    m_text->SetToolTip(toolTip);
    
    sizer->Add(m_text, 1, wxEXPAND | wxALIGN_CENTER | wxALL, 3);
}

void PropertyControl::setupMinMaxIntDoubleControls(wxSizer* sizer, 
                                                   wxString toolTip)
{
    assert(m_prop->hasMinMax() && "Property must have a min & max");

    // Find the proper range of values for the slider
    int minValue = 0;
    int maxValue = 100;
    int currentValue = 0;

    switch (m_prop->getType())
    {
        case core::Property::PT_INT:
            {
                minValue = boost::any_cast<int>(m_prop->getMinValue());
                maxValue = boost::any_cast<int>(m_prop->getMaxValue());
                currentValue = m_prop->getAsInt();
            }
            break;
        case core::Property::PT_DOUBLE:
            {
                minValue = (int)boost::any_cast<double>(m_prop->getMinValue());
                maxValue = (int)boost::any_cast<double>(m_prop->getMaxValue());

	        m_sliderScale = 100;
	        minValue = (int)(minValue * m_sliderScale);
	        maxValue = (int)(maxValue * m_sliderScale);
		currentValue = (int)(m_prop->getAsDouble() * m_sliderScale);
            }
            break;
        default:
            assert(false && "Error wrong property type");
            break;
    }
  
    // Create the slider
    m_slider = new wxSlider(this, wxID_ANY, currentValue, minValue, maxValue);
    m_slider->SetToolTip(toolTip);

    Connect(m_slider->GetId(), wxEVT_SCROLL_THUMBTRACK,
            wxScrollEventHandler(PropertyControl::onSliderUpdate));
    Connect(m_slider->GetId(), wxEVT_SCROLL_CHANGED,
            wxScrollEventHandler(PropertyControl::onSliderUpdate));

    sizer->Add(m_slider, 1, wxEXPAND | wxALIGN_CENTER | wxALL, 3);



    // Create the text box
    wxWindowDC temp(this);
    wxSize textSize = temp.GetTextExtent(_T("00000"));
    m_text = new wxTextCtrl(this, wxID_ANY, m_defaultValue, wxDefaultPosition, 
                            textSize, wxTE_RIGHT | wxTE_PROCESS_ENTER);
    m_text->SetToolTip(toolTip);
    sizer->Add(m_text, 0, wxEXPAND | wxALL, 3);
}

void PropertyControl::setupBoolControls(wxSizer* sizer, wxString toolTip)
{
    m_checkBox = new wxCheckBox(this, wxID_ANY, wxT(""));
    m_checkBox->SetValue(m_prop->getAsBool());
    m_checkBox->SetToolTip(toolTip);
  
    m_checkBox->Connect(m_checkBox->GetId(), 
                        wxEVT_COMMAND_CHECKBOX_CLICKED,
                        wxCommandEventHandler(PropertyControl::onCheck), NULL, 
                        this);
  
    m_checkBox->SetToolTip(toolTip);
  
    sizer->AddStretchSpacer();
    sizer->Add(m_checkBox, 0, wxALL, 3);
}

  
} // namespace visionvwr
} // namespace tools
} // namespace ram

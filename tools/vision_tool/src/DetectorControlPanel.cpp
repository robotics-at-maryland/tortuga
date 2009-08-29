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
#include <iostream>
#include <cstdlib>
#include <cstdio>
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

BEGIN_EVENT_TABLE(DetectorControlPanel, wxScrolledWindow)
END_EVENT_TABLE()
    
DetectorControlPanel::DetectorControlPanel(Model* model,
                                     wxWindow *parent, wxWindowID id,
                                     const wxPoint &pos, const wxSize &size) :
    wxScrolledWindow(parent, id, pos, size),
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

    // Button that will export all current values to the current file
    wxButton* exprt = new wxButton(this, wxID_ANY, wxT("Export File..."));

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
    sizer->Add(exprt, 0, wxALIGN_CENTER | wxALL, 3);

    sizer->SetSizeHints(this);
    SetSizer(sizer);

    setupScrolling();
    
    // Subscribe to model events
    m_model->subscribe(Model::DETECTOR_CHANGED,
        boost::bind(&DetectorControlPanel::onDetectorChanged, this, _1));

    // Connect to our events
    Connect(m_choice->GetId(), wxEVT_COMMAND_CHOICE_SELECTED,
            wxCommandEventHandler(DetectorControlPanel::onDetectorSelected));
    Connect(reset->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(DetectorControlPanel::onReset));
    Connect(exprt->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
	    wxCommandEventHandler(DetectorControlPanel::onExport));
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
    sizer->Layout();
    Refresh();
    Update();

    setupScrolling();
}

void DetectorControlPanel::onReset(wxCommandEvent& event)
{
    BOOST_FOREACH(PropertyControl* propControl, m_propControls)
    {
	propControl->setToDefault();
    }
}

void DetectorControlPanel::onExport(wxCommandEvent& event)
{
    //wxString filepath(wxT("tools/simulator/data/config/vision/default.yml"));
    //exportDetectorSettings(filepath);
    printf("Not implemented yet.\n");
}

void DetectorControlPanel::setupScrolling()
{
    // Parameters to tweak the algorithm
    bool scroll_x = true;
    bool scroll_y = true;
    int rate_x = 20;
    int rate_y = 20;

    if (!scroll_x)
        rate_x = 0;
    if (!scroll_y)
        rate_y = 0;

    // Round up the virtual size to be a multiple of the scroll rate
    wxSizer* sizer = GetSizer();
    wxSize minSize = sizer->GetMinSize();
    int w = minSize.GetWidth();
    int h = minSize.GetHeight();
    if (rate_x)
        w += rate_x - (w % rate_x);
    if (rate_y)
        h += rate_y - (h % rate_y);
    
    SetVirtualSize(minSize);
    SetScrollRate(rate_x, rate_y);
    SetVirtualSize(GetBestVirtualSize());
}

int DetectorControlPanel::exportDetectorSettings(wxString filename)
{
    // Get the name of the detector
    wxString detector(m_choice->GetString(m_choice->GetSelection()));

    // Find the name of the file, if there is no '/', it does nothing
    int pos = filename.Find(wxT('/'), true);
    wxString path;
    wxString name(filename);
    wxString tempFilePath;
    if (pos != -1)
    {
	path = filename.Left(pos + 1);
	name = filename.Right(filename.Len() - pos - 1);
    }
    tempFilePath = path + wxT('#') + name + wxT('#');

    // Open the file to be read and open a new temp file to be overwritten
    wxFile file(filename.c_str(), wxFile::read);
    wxFile tempFile(tempFilePath.c_str(), wxFile::write);

    if (file.IsOpened() && tempFile.IsOpened())
    {
	wxString line;
	while (!file.Eof()) {
	    // Read all lines and trim off any trailing spaces
	    line = readLine(file).Trim(true);
	    if (line.Contains(detector))
	    {
		// We've found the detector in the config file, skip over it
		line = readLine(file).Trim(true);
		while (!file.Eof() && line.StartsWith(wxT(" ")))
		    line = readLine(file).Trim(true);
	    }
	    else
	    {
		tempFile.Write(line + wxT("\n"));
	    }
	}

	// Now append the detector settings to the end of the file
	if (m_propControls.size() > 0)
	{
	    tempFile.Write(detector + wxT(":\n"));
	    BOOST_FOREACH(PropertyControl* propControl, m_propControls)
	    {
		tempFile.Write(wxT("    ") + propControl->getPropertyValue()
			       + wxT("\n"));
	    }
	}
	file.Close();
	tempFile.Close();

	// Now we're going to overwrite the old file with the new settings
	file.Open(filename.c_str(), wxFile::write);
	tempFile.Open(tempFilePath.c_str(), wxFile::read);

	if (file.IsOpened() && tempFile.IsOpened())
	{
	    wxString line;
	    while (!tempFile.Eof())
	    {
		line = readLine(tempFile);
		file.Write(line);
	    }
	}
	return 0;
    } else return 1;
}
    
} // namespace visionvwr
} // namespace tools
} // namespace ram

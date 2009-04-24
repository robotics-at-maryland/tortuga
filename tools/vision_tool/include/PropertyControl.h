/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:   tools/vision_viewer/include/PropertyControl.h
 */

#ifndef RAM_TOOLS_VISION_TOOL_PROPERTYCONTROL_H_04_23_2009
#define RAM_TOOLS_VISION_TOOL_PROPERTYCONTROL_H_04_23_2009

// Library Includes
#include <wx/panel.h>

// Project Includes
#include "core/include/Forward.h"

// Forward Declarations
class wxCommandEvent;
class wxTextCtrl;
class wxStaticText;

namespace ram {
namespace tools {
namespace visionvwr {
    
class PropertyControl : public wxPanel
{

public:
    PropertyControl(core::PropertyPtr property, wxWindow *parent,
		    wxWindowID id = wxID_ANY,
		    const wxPoint &pos = wxDefaultPosition,
		    const wxSize &size = wxDefaultSize);
    ~PropertyControl();
    
private:
    /** Called every time the text in the box changes */
    void onTextUpdated(wxCommandEvent& event);

    /** Called when the user presses enter in the text box */
    void onEnter(wxCommandEvent& event);

    /** Gets the value of the property as a string*/
    wxString getPropertyAsString();

    /** The property we are editing */
    core::PropertyPtr m_prop;

    /** Text box the users edits  */
    wxTextCtrl* m_text;

    /** Label for the detector */
    wxStaticText* m_label;
        
    DECLARE_EVENT_TABLE()
};

} // namespace visionvwr
} // namespace tools
} // namespace ram

#endif // RAM_TOOLS_VISION_TOOL_PROPERTYCONTROL_H_04_23_2009

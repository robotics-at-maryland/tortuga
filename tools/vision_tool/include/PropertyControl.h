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
#include "Forward.h"

// Forward Declarations
class wxCommandEvent;
class wxTextCtrl;
class wxStaticText;
class wxCheckBox;
class wxSlider;
class wxSizer;
class wxScrollEvent;

namespace ram {
namespace tools {
namespace visionvwr {
    
class PropertyControl : public wxPanel
{

public:
    PropertyControl(core::PropertyPtr property, Model* model,
                    wxWindow *parent, wxWindowID id = wxID_ANY,
		    const wxPoint &pos = wxDefaultPosition,
		    const wxSize &size = wxDefaultSize);
    ~PropertyControl();

    /** Returns the property to its default value */
    void setToDefault();

    /** Gets the current value in config file form
     *  name: value
     */
    wxString getPropertyValue();
    
private:
    /** Called every time the text in the box changes */
    void onTextUpdated(wxCommandEvent& event);

    /** Called when the user presses enter in the text box */
    void onEnter(wxCommandEvent& event);

    /** Handles the check box being checked on and off */
    void onCheck(wxCommandEvent& event);

    /** Called when the slider moves */
    void onSliderUpdate(wxScrollEvent& event);

    /** The overall property set has changed update our displayed values */
    void onPropertiesChanged(core::EventPtr event);

    /** Sets the property based on the given text */
    void setPropertyValue(wxString value);

    /** Sets up the non-slider int/double property controls  */
    void setupNormalIntDoubleControls(wxSizer* sizer, wxString toolTip);

    /** Sets up the slider based int/double property controls  */
    void setupMinMaxIntDoubleControls(wxSizer* sizer, wxString toolTip);

    /** Sets up the controls for bool property */
    void setupBoolControls(wxSizer* sizer, wxString toolTip);

    /** The property we are editing */
    core::PropertyPtr m_prop;

    /** Connection for the properties change event*/
    core::EventConnectionPtr m_propChangeConnection;

    /** Text box the users edits  */
    wxTextCtrl* m_text;

    /** Used for boolean properties */
    wxCheckBox* m_checkBox;

    /** Slider for changing property value */
    wxSlider* m_slider;

    /** Scales the value in the slider to the properties range */
    double m_sliderScale;

    /** Label for the detector */
    wxStaticText* m_label;

    /** Object running the detector */
    Model* m_model;
        
    /** The original value of the property */
    wxString m_defaultValue;

    DECLARE_EVENT_TABLE()
};

} // namespace visionvwr
} // namespace tools
} // namespace ram

#endif // RAM_TOOLS_VISION_TOOL_PROPERTYCONTROL_H_04_23_2009

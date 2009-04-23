/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:   tools/vision_viewer/include/DetectorControlPanel.h
 */

#ifndef RAM_TOOLS_VISION_TOOL_DETECTORCONTROLPANEL_H_04_23_2009
#define RAM_TOOLS_VISION_TOOL_DETECTORCONTROLPANEL_H_04_23_2009

// Library Includes
#include <wx/panel.h>

// Project Includes
#include "vision/include/Forward.h"
#include "core/include/Forward.h"

#include "Forward.h"

// Forward Declarations
class wxCommandEvent;
class wxChoice;

namespace ram {
namespace tools {
namespace visionvwr {
    
class DetectorControlPanel: public wxPanel
{

public:
    DetectorControlPanel(Model* model, wxWindow *parent,
			 wxWindowID id = wxID_ANY,
			 const wxPoint &pos = wxDefaultPosition,
			 const wxSize &size = wxDefaultSize);
    ~DetectorControlPanel();
    
private:
    /** When the user stops dragging the slider, or click on the slider*/
    void onDetectorChanged(wxCommandEvent& event);

    /** The object that is decoding the movie */
    Model* m_model;

    /** Selection box for the current detector */
    wxChoice* m_choice;
        
    DECLARE_EVENT_TABLE()
};

} // namespace visionvwr
} // namespace tools
} // namespace ram

#endif // RAM_TOOLS_VISION_TOOL_DETECTORCONTROLPANEL_H_04_23_2009

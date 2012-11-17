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

// STD Includes
#include <vector>

// Library Includes
#include <wx/scrolwin.h>
#include <wx/file.h>

// Project Includes
#include "vision/include/Forward.h"
#include "core/include/Forward.h"

#include "Forward.h"

// Forward Declarations
class wxCommandEvent;
class wxChoice;
class wxCloseEvent;

namespace ram {
namespace tools {
namespace visiontool {
    
class DetectorControlPanel: public wxScrolledWindow
{

public:
    DetectorControlPanel(Model* model, wxWindow *parent,
			 wxWindowID id = wxID_ANY,
			 const wxPoint &pos = wxDefaultPosition,
			 const wxSize &size = wxDefaultSize);
    ~DetectorControlPanel();
    
private:
    /** When the user selects a detector from the drop down box */
    void onDetectorSelected(wxCommandEvent& event);

    /** Called when the model says the detector changed */
    void onDetectorChanged(core::EventPtr event);

    /** Called when reset to defaults button is pressed */
    void onReset(wxCommandEvent& event);

    /** Called when export all settings button is pressed */
    void onExport(wxCommandEvent& event);

    /** Sets up all the needed scrolling parameters.
     *
     *  This function sets up the event handling necessary to handle
     *  scrolling properly. Its called whenever we change out the
     *  detector controllers.
     */
    void setupScrolling();

    /** Reads a line from the specified file
     *
     *  This function returns an empty string if the file is not opened
     *  and readable.
     */
    static wxString readLine(wxFile& file)
    {
	wxString val;
	if (file.IsOpened())
	{
	    void* block = calloc(1, sizeof(char));
	    if (block == NULL)
		return val;
	    char* buffer = (char *) block;

	    // This will check that the last character read was not a newline
	    // This will cause the newline to be read and stored in the string
	    while (*buffer != '\n' && file.Read(buffer, sizeof(char)))
	    {
		val += *buffer;
	    }
	}
	return val;
    }

    /** Exports the current detector to the given file */
    int exportDetectorSettings(wxString filename);

    void onSize(wxSizeEvent& sizeEvent);
    
    /** The object that is decoding the movie */
    Model* m_model;

    /** Selection box for the current detector */
    wxChoice* m_choice;

    /** The list of the current set of controls for the detectors properties */
    std::vector<PropertyControl*> m_propControls;
        
    DECLARE_EVENT_TABLE()
};

} // namespace visiontool
} // namespace tools
} // namespace ram

#endif // RAM_TOOLS_VISION_TOOL_DETECTORCONTROLPANEL_H_04_23_2009

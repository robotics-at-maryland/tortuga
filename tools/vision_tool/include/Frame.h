/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  tools/vision_tool/include/Frame.h
 */

#ifndef RAM_TOOLS_VISION_TOOL_FRAME_H_01_20_2008
#define RAM_TOOLS_VISION_TOOL_FRAME_H_01_20_2008

// Library Includes
#include <wx/frame.h>

// Project Includes
#include "Forward.h"

#include "vision/include/Forward.h"

// Forward Decl.
class wxTextCtrl;

namespace ram {
namespace tools {
namespace visionvwr {
    
enum
{
    ID_Quit = 1, ID_About, ID_OpenFile, ID_OpenCamera, ID_SaveImage,
    ID_SaveAsImage, ID_SetDir
};

class Frame: public wxFrame
{
public:

    Frame(const wxString& title, const wxPoint& pos, const wxSize& size);
    
private:
    void onQuit(wxCommandEvent& event);
    void onClose(wxCloseEvent& event);
    void onAbout(wxCommandEvent& event);
    void onOpenFile(wxCommandEvent& event);
    void onOpenCamera(wxCommandEvent& event);
    void onSetDirectory(wxCommandEvent& event);
    bool saveImage(wxString name, bool suppressError = false);
    void onSaveAsImage(wxCommandEvent& event);
    wxString numToString(int num);
    void onSaveImage(wxCommandEvent& event);
    void onShowHideDetector(wxCommandEvent& event);

    /** If possible it will hide the frame instead of showing closing */
    void onDetectorFrameClose(wxCloseEvent& event);

    void onSetConfigPath(wxCommandEvent& event);

    MediaControlPanel* m_mediaControlPanel;
    GLMovie* m_movie;
    Model* m_model;
    wxFrame* m_detectorFrame;
    wxTextCtrl* m_configText;

    wxString m_saveDir;
    int m_idNum;

    DECLARE_EVENT_TABLE()
};

} // namespace visionvwr
} // namespace tools
} // namespace ram

#endif // RAM_TOOLS_VISION_TOOL_FRAME_H_01_20_2008

/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders88@gmail.com>
 * File:  tools/vision_tool/include/Frame.h
 */

#ifndef RAM_TOOLS_VISIONTOOL_FRAME_H
#define RAM_TOOLS_VISIONTOOL_FRAME_H

// Library Includes
#include <wx/frame.h>
#include <wx/window.h>

// Project Includes
#include "Forward.h"

#include "vision/include/Forward.h"

// Forward Decl.
class wxTextCtrl;

namespace ram {
namespace tools {
namespace visiontool {
    
enum
{
    ID_Quit = 1, ID_About, ID_OpenFile, ID_OpenCamera,
    ID_OpenForwardCamera, ID_OpenDownwardCamera, ID_SaveImage,
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
    void onOpenForwardCamera(wxCommandEvent& event);
    void onOpenDownwardCamera(wxCommandEvent& event);
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
    wxWindow* m_rawMovie;
    wxWindow* m_detectorMovie;

    wxWindow *m_ch1Movie, *m_ch2Movie, *m_ch3Movie;
    wxWindow *m_ch1HistMovie, *m_ch2HistMovie, *m_ch3HistMovie;
    wxWindow *m_ch12HistMovie, *m_ch23HistMovie, *m_ch13HistMovie;

    Model* m_model;
    wxFrame* m_detectorFrame;
    wxTextCtrl* m_configText;

    wxString m_saveDir;
    int m_idNum;

    DECLARE_EVENT_TABLE()
};

} // namespace visiontool
} // namespace tools
} // namespace ram

#endif // RAM_TOOLS_VISIONTOOL_FRAME_H

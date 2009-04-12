/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  tools/vision_tool/src/App.cpp
 */

// This include must be first because of Mac header defines
#include "vision/include/OpenCVCamera.h"
#include "vision/include/FFMPEGCamera.h"

// Library Includes
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/timer.h>

// Project Includes
#include "Frame.h"
#include "GLMovie.h"
#include "MediaControlPanel.h"
#include "GLMovie.h"

namespace ram {
namespace tools {
namespace visionvwr {

static int ID_TIMER = 5;    
BEGIN_EVENT_TABLE(Frame, wxFrame)
    EVT_MENU(ID_Quit, Frame::onQuit)
    EVT_MENU(ID_About, Frame::onAbout)
    EVT_MENU(ID_OpenFile, Frame::onOpenFile)
    EVT_MENU(ID_OpenCamera, Frame::onOpenCamera)
    EVT_TIMER(ID_TIMER, Frame::onTimer)
END_EVENT_TABLE()


Frame::Frame(const wxString& title, const wxPoint& pos, const wxSize& size) :
    wxFrame((wxFrame *)NULL, -1, title, pos, size),
    m_mediaControlPanel(0),
    m_movie (0),
    m_camera(0)
{
    // File Menu
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_OpenFile, _T("Open Video &File"));
    menuFile->Append(ID_OpenCamera, _T("Open CV &Camera"));
    menuFile->Append(ID_About, _T("&About..."));
    menuFile->AppendSeparator();
    menuFile->Append(ID_Quit, _T("E&xit"));
    
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, _T("&File") );
    
    SetMenuBar( menuBar );

    // Timer which drives the process
    m_timer = new wxTimer(this, ID_TIMER);
    
    // Add CameraView panel full screen
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    m_movie=new GLMovie(this);
    sizer->Add(m_movie, 1, wxEXPAND, 0);
    m_mediaControlPanel = new MediaControlPanel(m_timer, this);
    sizer->Add(m_mediaControlPanel, 0, wxEXPAND, 0);
    sizer->SetSizeHints(this);
    SetSizer(sizer);
}

void Frame::onQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}
    
void Frame::onAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("An application to view live or recored video"),
                 _T("About Vision Viewer"), wxOK | wxICON_INFORMATION, this);
}

void Frame::onOpenFile(wxCommandEvent& event)
{
    wxString filename = wxFileSelector(_T("Choose a video file to open"));
    if ( !filename.empty() )
    {
        if (m_camera)
            delete m_camera;
        
        m_camera = new vision::FFMPEGCamera(std::string(filename.mb_str()));
        m_movie->setCamera(m_camera);
        m_mediaControlPanel->setCamera(m_camera);
        m_movie->nextFrame();
    }
}
    
void Frame::onOpenCamera(wxCommandEvent& event)
{
    if (m_camera)
        delete m_camera;
    
    m_camera = new vision::OpenCVCamera();
    m_movie->setCamera(m_camera);
    m_mediaControlPanel->setCamera(m_camera);
}

void Frame::onTimer(wxTimerEvent &event)
{
    m_movie->nextFrame();
    m_movie->Refresh();
    
    m_mediaControlPanel->update();
}
    

} // namespace visionvwr
} // namespace tools
} // namespace ram

/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  tools/vision_viewer/src/App.cpp
 */

// Library Includes
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/msgdlg.h>

// Project Includes
#include "Frame.h"
#include "CameraView.h"

namespace ram {
namespace tools {
namespace visionvwr {

BEGIN_EVENT_TABLE(Frame, wxFrame)
    EVT_MENU(ID_Quit, Frame::onQuit)
    EVT_MENU(ID_About, Frame::onAbout)
END_EVENT_TABLE()


Frame::Frame(const wxString& title, const wxPoint& pos, const wxSize& size) :
    wxFrame((wxFrame *)NULL, -1, title, pos, size),
    m_view(0)
{
    wxMenu *menuFile = new wxMenu;

    menuFile->Append( ID_About, _T("&About...") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_Quit, _T("E&xit") );

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, _T("&File") );

    SetMenuBar( menuBar );
    
    // Add CameraView panel full screen
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    m_view = new CameraView(this); //, _T("images.jpg"));
    sizer->Add(m_view, 1, wxEXPAND);
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

} // namespace visionvwr
} // namespace tools
} // namespace ram

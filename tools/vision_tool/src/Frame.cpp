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
#include <wx/filedlg.h>

// Project Includes
#include "Frame.h"
#include "GLMovie.h"
#include "MediaControlPanel.h"
#include "vision/include/OpenCVCamera.h"
#include "GLMovie.h"

namespace ram
{
namespace tools
{
namespace visionvwr
{
BEGIN_EVENT_TABLE(Frame, wxFrame)
EVT_MENU(ID_Quit, Frame::onQuit)
    EVT_MENU(ID_About, Frame::onAbout)
    EVT_MENU(ID_OpenFile, Frame::onOpenFile)
    EVT_MENU(ID_OpenCamera, Frame::onOpenCamera)
END_EVENT_TABLE()


Frame::Frame(const wxString& title, const wxPoint& pos, const wxSize& size) :
    wxFrame((wxFrame *)NULL, -1, title, pos, size),
    mediaControlPanel(0), movie (0)
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

		// Add CameraView panel full screen
		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		this->movie=new GLMovie(this);
		sizer->Add(this->movie, 1, wxEXPAND, 0);
		this->mediaControlPanel = new MediaControlPanel(this->movie, this);
		sizer->Add(this->mediaControlPanel, 0, wxEXPAND, 0);
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
			vision::OpenCVCamera* camera =
			new vision::OpenCVCamera(std::string(filename.mb_str()));
			this->movie->setCamera(camera);
			this->movie->nextFrame();
		}
	}

	void Frame::onOpenCamera(wxCommandEvent& event)
	{
		vision::OpenCVCamera* camera = new vision::OpenCVCamera();
		movie->setCamera(camera);
	}

} // namespace visionvwr
} // namespace tools
} // namespace ram

/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  tools/vision_tool/src/App.cpp
 */

// STD Includes
#include <iostream>

// Library Includes
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/timer.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/utils.h>
#include <wx/filename.h>
#include <wx/stattext.h>

// For cvSaveImage
#include "highgui.h"

// Project Includes
#include "Frame.h"
#include "IPLMovie.h"
#include "MediaControlPanel.h"
#include "DetectorControlPanel.h"
#include "Model.h"
#include "vision/include/Image.h"

namespace ram {
namespace tools {
namespace visiontool {

BEGIN_EVENT_TABLE(Frame, wxFrame)
    EVT_MENU(ID_Quit, Frame::onQuit)
    EVT_MENU(ID_About, Frame::onAbout)
    EVT_MENU(ID_OpenFile, Frame::onOpenFile)
    EVT_MENU(ID_OpenCamera, Frame::onOpenCamera)
    EVT_MENU(ID_OpenForwardCamera, Frame::onOpenForwardCamera)
    EVT_MENU(ID_OpenDownwardCamera, Frame::onOpenDownwardCamera)
    EVT_MENU(ID_SetDir, Frame::onSetDirectory)
    EVT_MENU(ID_SaveImage, Frame::onSaveImage)
    EVT_MENU(ID_SaveAsImage, Frame::onSaveAsImage)
END_EVENT_TABLE()


Frame::Frame(const wxString& title, const wxPoint& pos, const wxSize& size) :
    wxFrame((wxFrame *)NULL, -1, title, pos, size),
    m_mediaControlPanel(0),
    m_rawMovie(0),
    m_detectorMovie(0),
    m_model(new Model),
    m_idNum(1)
{
    // File Menu
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_OpenFile, _T("Open Video &File"));
    menuFile->Append(ID_OpenCamera, _T("Open Local CV &Camera"));
    menuFile->Append(ID_OpenForwardCamera, _T("Open tortuga4:50000"));
    menuFile->Append(ID_OpenDownwardCamera, _T("Open tortuga4:50001"));
    menuFile->Append(ID_About, _T("&About..."));
    menuFile->AppendSeparator();
    menuFile->Append(ID_Quit, _T("E&xit\tCtrl+Q"));

    // Image Menu
    wxMenu *menuImage = new wxMenu;
    menuImage->Append(ID_SetDir, _T("Set &Directory..."));
    menuImage->AppendSeparator();
    menuImage->Append(ID_SaveImage, _T("&Save\tCtrl+S"));
    menuImage->Append(ID_SaveAsImage, _T("Save &Image..."));

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, _T("&File") );
    menuBar->Append( menuImage, _T("&Image") );
    
    SetMenuBar( menuBar );

    // Create our controls
    m_mediaControlPanel = new MediaControlPanel(m_model, this);
    m_rawMovie = new IPLMovie(this, m_model, Model::NEW_RAW_IMAGE);
    m_detectorMovie = new IPLMovie(this, m_model, Model::NEW_PROCESSED_IMAGE);
    m_ch1Movie = new IPLMovie(this, m_model, Model::NEW_CH1_IMAGE, wxSize(320, 240));
    m_ch2Movie = new IPLMovie(this, m_model, Model::NEW_CH2_IMAGE, wxSize(320, 240));
    m_ch3Movie = new IPLMovie(this, m_model, Model::NEW_CH3_IMAGE, wxSize(320, 240));
    m_ch1HistMovie = new IPLMovie(this, m_model, Model::NEW_CH1_HIST_IMAGE,
                                  wxSize(120, 120));
    m_ch2HistMovie = new IPLMovie(this, m_model, Model::NEW_CH2_HIST_IMAGE,
                                  wxSize(120, 120));
    m_ch3HistMovie = new IPLMovie(this, m_model, Model::NEW_CH3_HIST_IMAGE,
                                  wxSize(120, 120));
    m_ch12HistMovie = new IPLMovie(this, m_model, Model::NEW_CH12_HIST_IMAGE,
                                   wxSize(120, 120));
    m_ch23HistMovie = new IPLMovie(this, m_model, Model::NEW_CH23_HIST_IMAGE,
                                   wxSize(120, 120));
    m_ch13HistMovie = new IPLMovie(this, m_model, Model::NEW_CH13_HIST_IMAGE,
                                   wxSize(120, 120));

    wxButton* config = new wxButton(this, wxID_ANY, wxT("Config File"));
    wxString defaultPath;
    wxGetEnv(_T("RAM_SVN_DIR"), &defaultPath);
    m_configText = new wxTextCtrl(this, wxID_ANY, defaultPath,
                                  wxDefaultPosition, wxDefaultSize, 
                                  wxTE_READONLY);
    wxButton* detectorHide = new wxButton(this, wxID_ANY, 
                                          wxT("Show/Hide Detector"));

    // Place controls in the sizer
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_mediaControlPanel, 0, wxEXPAND, 0);

    wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
    row->Add(config, 0, wxALL, 3);
    row->Add(m_configText, 1, wxALIGN_CENTER | wxTOP | wxBOTTOM, 3);
    row->Add(detectorHide, 0, wxALL, 3);
    sizer->Add(row, 0, wxEXPAND, 0);

    wxBoxSizer* movieTitleSizer = new wxBoxSizer(wxHORIZONTAL);
    movieTitleSizer->Add(
        new wxStaticText(this, wxID_ANY, _T("Raw Image")),
        1, wxEXPAND | wxALL, 2);
    movieTitleSizer->Add(
        new wxStaticText(this, wxID_ANY, _T("Detector Debug Output")),
        1, wxEXPAND | wxALL, 2);

    m_rawMovie->SetWindowStyle(wxBORDER_RAISED);
    m_detectorMovie->SetWindowStyle(wxBORDER_RAISED);
    wxBoxSizer* movieSizer = new wxBoxSizer(wxHORIZONTAL);
    movieSizer->Add(m_rawMovie, 1, wxSHAPED | wxALL, 2);
    movieSizer->Add(m_detectorMovie, 1, wxSHAPED | wxALL, 2);


    sizer->Add(movieTitleSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
    sizer->Add(movieSizer, 4, wxEXPAND | wxLEFT | wxRIGHT, 5);

    // Single Channel Histogram Sizers
    wxBoxSizer *ch1HistSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText *ch1HistText = new wxStaticText(this, wxID_ANY, _T("Ch1 Hist"));
    ch1HistSizer->Add(ch1HistText, 0, wxEXPAND | wxALL, 1);
    ch1HistSizer->Add(m_ch1HistMovie, 1, wxSHAPED);

    wxBoxSizer *ch2HistSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText *ch2HistText = new wxStaticText(this, wxID_ANY, _T("Ch2 Hist"));
    ch2HistSizer->Add(ch2HistText, 0, wxEXPAND | wxALL, 1);
    ch2HistSizer->Add(m_ch2HistMovie, 1, wxSHAPED);

    wxBoxSizer *ch3HistSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText *ch3HistText = new wxStaticText(this, wxID_ANY, _T("Ch3 Hist"));
    ch3HistSizer->Add(ch3HistText, 0, wxEXPAND | wxALL, 1);
    ch3HistSizer->Add(m_ch3HistMovie, 1, wxSHAPED);

    // 2D Histogram Sizers
    wxBoxSizer *ch12HistSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText *ch12HistText = new wxStaticText(this, wxID_ANY, _T("Ch1 vs. Ch2 Hist"));
    ch12HistSizer->Add(ch12HistText, 0, wxEXPAND | wxALL, 1);
    ch12HistSizer->Add(m_ch12HistMovie, 1, wxSHAPED);

    wxBoxSizer *ch23HistSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText *ch23HistText = new wxStaticText(this, wxID_ANY, _T("Ch2 vs. Ch3 Hist"));
    ch23HistSizer->Add(ch23HistText, 0, wxEXPAND | wxALL, 1);
    ch23HistSizer->Add(m_ch23HistMovie, 1, wxSHAPED);

    wxBoxSizer *ch13HistSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText *ch13HistText = new wxStaticText(this, wxID_ANY, _T("Ch1 vs. Ch3 Hist"));
    ch13HistSizer->Add(ch13HistText, 0, wxEXPAND | wxALL, 1);
    ch13HistSizer->Add(m_ch13HistMovie, 1, wxSHAPED);

    // Overall Sizer for Histograms
    wxFlexGridSizer *histImgMovieSizer = new wxFlexGridSizer(2, 3, 0, 0);
    histImgMovieSizer->Add(ch1HistSizer, 1, wxEXPAND | wxALL, 1);
    histImgMovieSizer->Add(ch2HistSizer, 1, wxEXPAND | wxALL, 1);
    histImgMovieSizer->Add(ch3HistSizer, 1, wxEXPAND | wxALL, 1);
    histImgMovieSizer->Add(ch12HistSizer, 1, wxEXPAND | wxALL, 1);
    histImgMovieSizer->Add(ch23HistSizer, 1, wxEXPAND | wxALL, 1);
    histImgMovieSizer->Add(ch13HistSizer, 1, wxEXPAND | wxALL, 1);

    // Individual Channel Image Sizers
    wxBoxSizer *ch1Sizer = new wxBoxSizer(wxVERTICAL);
    ch1Sizer->Add(
        new wxStaticText(this, wxID_ANY, _T("Channel 1"), wxDefaultPosition,
                         wxDefaultSize, wxALIGN_LEFT),
        0, wxEXPAND | wxLEFT | wxRIGHT, 5);
    ch1Sizer->Add(m_ch1Movie, 1, wxSHAPED);

    wxBoxSizer *ch2Sizer = new wxBoxSizer(wxVERTICAL);
    ch2Sizer->Add(
        new wxStaticText(this, wxID_ANY, _T("Channel 2"), wxDefaultPosition,
                         wxDefaultSize, wxALIGN_LEFT),
        0, wxEXPAND | wxLEFT | wxRIGHT, 5);
    ch2Sizer->Add(m_ch2Movie, 1, wxSHAPED);

    wxBoxSizer *ch3Sizer = new wxBoxSizer(wxVERTICAL);
    ch3Sizer->Add(
        new wxStaticText(this, wxID_ANY, _T("Channel 3"), wxDefaultPosition,
                         wxDefaultSize, wxALIGN_LEFT),
        0, wxEXPAND | wxLEFT | wxRIGHT, 5);
    ch3Sizer->Add(m_ch3Movie, 1, wxSHAPED);
        
    wxBoxSizer *smallMovieSizer = new wxBoxSizer(wxHORIZONTAL);
    smallMovieSizer->Add(ch1Sizer, 1, wxSHAPED | wxALL, 2);
    smallMovieSizer->Add(ch2Sizer, 1, wxSHAPED | wxALL, 2);
    smallMovieSizer->Add(ch3Sizer, 1, wxSHAPED | wxALL, 2);
    smallMovieSizer->Add(histImgMovieSizer, 1, wxEXPAND | wxALL, 2);

    sizer->Add(smallMovieSizer, 2, wxEXPAND | wxRIGHT | wxLEFT | wxBOTTOM, 5);
    sizer->SetSizeHints(this);
    SetSizer(sizer);

    // Create the seperate frame for the detector panel
    wxPoint framePosition = GetPosition();
    framePosition.x += GetSize().GetWidth();
    wxSize frameSize(GetSize().GetWidth()/3, GetSize().GetHeight());

    m_detectorFrame = 
      new wxFrame(this, wxID_ANY, _T("Detector Control"),
                  framePosition, frameSize);

    // Add a sizer and the detector control panel to it
    sizer = new wxBoxSizer(wxVERTICAL);
    wxPanel* detectorControlPanel = new DetectorControlPanel(
        m_model, m_detectorFrame);
    sizer->Add(detectorControlPanel, 1, wxEXPAND, 0);

    m_detectorFrame->SetSizer(sizer);
    m_detectorFrame->Show();

    // Register for events
    Connect(detectorHide->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(Frame::onShowHideDetector));
    Connect(config->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(Frame::onSetConfigPath));
    //    Connect(GetId(), wxEVT_CLOSE_WINDOW,
    //	    wxCloseEventHandler(Frame::onClose), this);
    //    Connect(m_detectorFrame->GetId(), wxEVT_CLOSE_WINDOW,
    //	    wxCloseEventHandler(Frame::onDetectorFrameClose), this);
}

void Frame::onQuit(wxCommandEvent& WXUNUSED(event))
{
    m_model->stop();    
    Close(true);
}

void Frame::onClose(wxCloseEvent& event)
{
    // Stop video playback when we shut down
    Destroy();
}
    
void Frame::onAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("An application to view live or recored video"),
                 _T("About Vision Tool"), wxOK | wxICON_INFORMATION, this);
}

void Frame::onOpenFile(wxCommandEvent& event)
{
    wxString filepath = wxFileSelector(_T("Choose a video file to open"));
    if ( !filepath.empty() )
    {
        // Have the model open the file
        m_model->openFile(std::string(filepath.mb_str()));

	// Place the file name in the title bar
	wxString filename;
	wxString extension;
	wxFileName::SplitPath(filepath, NULL, &filename, &extension);
	SetTitle(wxString(_T("Vision Tool - ")) + filename + _T(".") + 
		 extension);
    }
}
    
void Frame::onOpenCamera(wxCommandEvent& event)
{
    m_model->openCamera();
}

void Frame::onOpenForwardCamera(wxCommandEvent& event)
{
    m_model->openNetworkCamera("tortuga4", 50000);
}

void Frame::onOpenDownwardCamera(wxCommandEvent& event)
{
    m_model->openNetworkCamera("tortuga4", 50001);
}

void Frame::onSetDirectory(wxCommandEvent& event)
{
    wxDirDialog chooser(this);
    int id = chooser.ShowModal();
    if (id == wxID_OK) {
	m_saveDir = chooser.GetPath();
        m_idNum = 1;
    }
}

bool Frame::saveImage(wxString pathname, bool suppressError)
{
    vision::Image* image = m_model->getLatestImage();
    image->setPixelFormat(ram::vision::Image::PF_BGR_8);
    // Check that there is an image to save
    if (image == NULL) {
        // Don't create the message dialog if we suppress the window
        if (suppressError)
            return false;
	// Create a message saying there was an error saving
	wxMessageDialog messageWindow(this,
				      _T("Error: Could not save the image.\n"
					 "(The video cannot be playing while "
					 "saving an image)"),
				      _T("ERROR!"), wxOK);
	messageWindow.ShowModal();
	return false;
    }

    // No error, save the image
    image->setPixelFormat(ram::vision::Image::PF_BGR_8);
    cvSaveImage(pathname.mb_str(wxConvUTF8),
		image->asIplImage());
    return true;
}

void Frame::onSaveAsImage(wxCommandEvent& event)
{
    wxFileDialog saveWindow(this, _T("Save file..."), _T(""), _T(""),
			    _T("*.*"), wxSAVE | wxOVERWRITE_PROMPT);
    int result = saveWindow.ShowModal();
    if (result == wxID_OK) {
	saveImage(saveWindow.GetPath());
    }
}

wxString Frame::numToString(int num)
{
    wxString s = wxString::Format(_T("%d"), num);
    short zeros = 4 - s.length();

    wxString ret;
    ret.Append(_T('0'), zeros);
    ret.Append(s);
    ret.Append(_T(".png"));

    return ret;
}

void Frame::onSaveImage(wxCommandEvent& event)
{
    // Save to the default path
    // Find a file that doesn't exist
    wxFileName filename(m_saveDir, numToString(m_idNum));
    while (filename.FileExists()) {
	m_idNum++;
	filename = wxFileName(m_saveDir, numToString(m_idNum));
    }

    wxString fullpath(filename.GetFullPath());
    std::cout << "Quick save to " << fullpath.mb_str(wxConvUTF8) << std::endl;
    // Save image and suppress any error to be handled by this function
    bool ret = saveImage(fullpath, true);
    if (!ret) {
        std::cerr << "Quick save failed. Try stopping the video.\n";
    }
}

void Frame::onShowHideDetector(wxCommandEvent& event)
{
    // Toggle the shown status of the frame
    m_detectorFrame->Show(!m_detectorFrame->IsShown());
}

void Frame::onDetectorFrameClose(wxCloseEvent& event)
{
    // If we don't have to close, just hide the window
    if (event.CanVeto())
    {
        m_detectorFrame->Hide();
	event.Veto();
    }
    else
    {
        m_detectorFrame->Destroy();
    }
}
    
void Frame::onSetConfigPath(wxCommandEvent& event)
{
    wxString filename = wxFileSelector(_T("Choose a config file"),
				       m_configText->GetValue());
    if ( !filename.empty() )
    {
        m_configText->SetValue(filename);
        m_model->setConfigPath(std::string(filename.mb_str()));
    }
}

} // namespace visiontool
} // namespace tools
} // namespace ram

#include "MediaControlPanel.h"
#include "CameraView.h"

namespace ram {
namespace tools {
namespace visionvwr {

BEGIN_EVENT_TABLE(MediaControlPanel, wxPanel)
	EVT_BUTTON(MediaControlPanel::MEDIA_CONTROL_PANEL_BUTTON_PLAY, MediaControlPanel::onPlay)
	EVT_BUTTON(MediaControlPanel::MEDIA_CONTROL_PANEL_BUTTON_STOP, MediaControlPanel::onStop)
END_EVENT_TABLE()

MediaControlPanel::MediaControlPanel(CameraView *controlledView, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size) : wxPanel(parent, id, pos, size)
{
	this->controlledView=controlledView;

	this->play=new wxButton(this, MEDIA_CONTROL_PANEL_BUTTON_PLAY, wxT("Play"));
	this->stop=new wxButton(this, MEDIA_CONTROL_PANEL_BUTTON_STOP, wxT("Stop"));

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(this->play, 0, 0, 0);
	sizer->Add(this->stop, 0, 0, 0);
	sizer->SetSizeHints(this);
	SetSizer(sizer);
}
MediaControlPanel::~MediaControlPanel()
{
}

void MediaControlPanel::MediaControlPanel::onPlay(wxCommandEvent& event)
{
	this->controlledView->m_timer->Start(100);
}
void MediaControlPanel::MediaControlPanel::onStop(wxCommandEvent& event)
{
	this->controlledView->m_timer->Stop();
}

}
}
}

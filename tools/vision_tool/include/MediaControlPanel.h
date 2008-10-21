#ifndef MEDIA_CONTROL_PANEL_H
#define MEDIA_CONTROL_PANEL_H

#include <wx/panel.h>
#include <wx/button.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>

namespace ram
{
namespace tools
{
namespace visionvwr
{

class GLMovie;

class MediaControlPanel: public wxPanel
{
	private:
		wxButton *play;
		wxButton *stop;

		GLMovie *controlledMovie;

		enum MEDIA_CONTROL_PANEL_BUTTON_IDS
		{
			MEDIA_CONTROL_PANEL_BUTTON_PLAY, MEDIA_CONTROL_PANEL_BUTTON_STOP
		};

		void onPlay(wxCommandEvent& event);
		void onStop(wxCommandEvent& event);
	protected:
	public:
		MediaControlPanel(GLMovie *controlledMovie, wxWindow *parent,
				wxWindowID id = wxID_ANY, const wxPoint &pos =
						wxDefaultPosition, const wxSize &size = wxDefaultSize);
		~MediaControlPanel();

DECLARE_EVENT_TABLE()
};
}
}
}

#endif

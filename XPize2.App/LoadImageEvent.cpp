#include <wx/wx.h>
#include "LoadImageEvent.h"
#include "Constants.h"

LoadImageEvent::LoadImageEvent(wxEventType eventType, int winid, const wxString& path)
	: wxEvent(winid, eventType), m_path(path)
{

}

wxDEFINE_EVENT(APP_EVT_LOAD_IMAGE, LoadImageEvent);

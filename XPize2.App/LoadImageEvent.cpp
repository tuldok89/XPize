#include "LoadImageEvent.h"

LoadImageEvent::LoadImageEvent(wxEventType eventType, int winid, const wxString& path)
	: wxEvent(winid, eventType), m_path(path)
{
}

DEFINE_LOCAL_EVENT_TYPE(APP_EVT_LOAD_IMAGE)
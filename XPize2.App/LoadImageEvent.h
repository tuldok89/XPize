#pragma once

#include <wx/event.h>

class wxPoint;

class LoadImageEvent : public wxEvent
{
public:
	LoadImageEvent(wxEventType eventType, int winid, const wxString& path);
	wxString GetPath() const { return m_path; }
	virtual wxEvent* Clone() const override { return new LoadImageEvent(*this); }
private:
	wxString m_path;
};

wxDECLARE_EVENT(APP_EVT_LOAD_IMAGE, LoadImageEvent);
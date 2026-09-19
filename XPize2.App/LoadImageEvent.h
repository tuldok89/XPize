#pragma once
#include <wx/event.h>
#include <wx/string.h>

// wxWidgets 2.8 compatibility: wxDECLARE_EVENT() does not exist, use
// DECLARE_LOCAL_EVENT_TYPE()/DEFINE_LOCAL_EVENT_TYPE() instead.
DECLARE_LOCAL_EVENT_TYPE(APP_EVT_LOAD_IMAGE, 3000)

// wxWidgets 2.8's EVT_CUSTOM cannot dispatch to a handler taking a derived
// event type with gcc, so use a C-style cast like the built-in typed handlers.
#define EVT_LOAD_IMAGE(fn) \
	DECLARE_EVENT_TABLE_ENTRY(APP_EVT_LOAD_IMAGE, wxID_ANY, wxID_ANY, \
		(wxObjectEventFunction) (wxEventFunction) &fn, NULL),

class LoadImageEvent : public wxEvent
{
public:
	LoadImageEvent(wxEventType eventType, int winid, const wxString& path);
	wxString GetPath() const { return m_path; }
	virtual wxEvent* Clone() const { return new LoadImageEvent(*this); }
private:
	wxString m_path;
};
#pragma once
#include <wx/event.h>

DECLARE_LOCAL_EVENT_TYPE(APP_EVT_NAVIGATION, 3002)

// See the note in LoadImageEvent.h about wx 2.8 EVT_CUSTOM.
#define EVT_NAVIGATION(fn) \
	DECLARE_EVENT_TABLE_ENTRY(APP_EVT_NAVIGATION, wxID_ANY, wxID_ANY, \
		(wxObjectEventFunction) (wxEventFunction) &fn, NULL),

class NavigationEvent : public wxEvent
{
public:
	NavigationEvent(wxEventType eventType, int winid, const int page, const int total);
	int GetPage() const { return m_page; }
	int GetTotal() const { return m_total; }
	virtual wxEvent* Clone() const { return new NavigationEvent(*this); }
private:
	int m_page;
	int m_total;
};
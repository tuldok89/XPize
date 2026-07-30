#pragma once

#include <wx/event.h>

class NavigationEvent : public wxEvent
{
public:
	NavigationEvent(wxEventType eventType, int winid, const int page, const int total);
	int GetPage() const { return m_page; }
	int GetTotal() const { return m_total; }
	virtual wxEvent* Clone() const override { return new NavigationEvent(*this); }
private:
	int m_page;
	int m_total;
};

wxDECLARE_EVENT(APP_EVT_NAVIGATION, NavigationEvent);
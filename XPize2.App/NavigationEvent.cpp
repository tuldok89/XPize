#include "NavigationEvent.h"

NavigationEvent::NavigationEvent(wxEventType eventType, int winid, const int page, const int total) : wxEvent(winid, eventType), m_page(page), m_total(total)
{

}

wxDEFINE_EVENT(APP_EVT_NAVIGATION, NavigationEvent);
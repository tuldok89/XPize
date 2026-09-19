#pragma once
#include <wx/statusbr.h>

class wxString;
class wxWindow;
class NavigationEvent;

class StatusBar : public wxStatusBar
{
public:
	StatusBar(wxString initialText, wxWindow* parent);
	void OnNavigation(NavigationEvent& event);

	DECLARE_EVENT_TABLE()
};
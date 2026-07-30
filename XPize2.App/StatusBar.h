#pragma once

class wxStatusBar;
class wxString;
class NavigationEvent;
class wxDPIChangedEvent;
class StatusBar : public wxStatusBar
{
public:
	StatusBar(wxString initialText, wxWindow* parent);
	void OnNavigation(NavigationEvent& event);
	void OnDPIChanged(wxDPIChangedEvent& event);
private:
	void RecalculateFieldWidths();
};


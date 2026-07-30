#include <wx/statusbr.h>
#include <wx/string.h>
#include <wx/dcclient.h>
#include "NavigationEvent.h"
#include "StatusBar.h"

StatusBar::StatusBar(wxString initialText, wxWindow* parent) : wxStatusBar(parent)
{
	Bind(APP_EVT_NAVIGATION, &StatusBar::OnNavigation, this);
	SetFieldsCount(2);
	SetStatusWidths(2, new int[2] { -1, 200});
	SetStatusText(initialText);
	Bind(wxEVT_DPI_CHANGED, &StatusBar::OnDPIChanged, this);
}

void StatusBar::OnNavigation(NavigationEvent& event)
{
	auto text = wxString::Format(wxT("Page %d of %d"), event.GetPage(), event.GetTotal());
	SetStatusText(text, 1);
	wxClientDC dc(this);
	dc.SetFont(GetFont());
	auto extent = dc.GetTextExtent(text);
	SetStatusWidths(2, new int[2] { -1, extent.x + 8 });
}

void StatusBar::OnDPIChanged(wxDPIChangedEvent& event)
{
	RecalculateFieldWidths();
}

void StatusBar::RecalculateFieldWidths()
{
	auto text = GetStatusText(1);
	wxClientDC dc(this);
	dc.SetFont(GetFont());
	auto extent = dc.GetTextExtent(text);
	SetStatusWidths(2, new int[2] { -1, extent.x + 8 });
}

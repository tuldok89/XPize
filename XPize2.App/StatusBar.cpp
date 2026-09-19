#include <wx/statusbr.h>
#include <wx/string.h>
#include <wx/dcclient.h>
#include "NavigationEvent.h"
#include "StatusBar.h"

BEGIN_EVENT_TABLE(StatusBar, wxStatusBar)
	EVT_NAVIGATION(StatusBar::OnNavigation)
END_EVENT_TABLE()

StatusBar::StatusBar(wxString initialText, wxWindow* parent) : wxStatusBar(parent)
{
	SetFieldsCount(2);
	int widths[2];
	widths[0] = -1;
	widths[1] = 200;
	SetStatusWidths(2, widths);
	SetStatusText(initialText);
}

void StatusBar::OnNavigation(NavigationEvent& event)
{
	wxString text;
	text.Printf(wxT("Page %d of %d"), event.GetPage(), event.GetTotal());
	SetStatusText(text, 1);

	wxClientDC dc(this);
	dc.SetFont(GetFont());
	wxSize extent = dc.GetTextExtent(text);
	int widths[2];
	widths[0] = -1;
	widths[1] = extent.x + 8;
	SetStatusWidths(2, widths);
}
#include <wx/wx.h>
#include <wx/statbmp.h>

#include "Scroller.h"
#include "LoadImageEvent.h"

BEGIN_EVENT_TABLE(Scroller, wxScrolledWindow)
	EVT_LOAD_IMAGE(Scroller::OnLoadImage)
END_EVENT_TABLE()

Scroller::Scroller(wxWindow* parent)
	: wxScrolledWindow(parent, wxID_ANY)
{
	SetScrollRate(64, 64);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	m_imageControl = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap);
	sizer->Add(m_imageControl, 1, wxEXPAND, 0);
	SetSizer(sizer);
}

void Scroller::OnLoadImage(LoadImageEvent& event)
{
	wxImage image;
	if (!image.LoadFile(event.GetPath()))
	{
		wxMessageBox(wxT("Failed to load image: ") + event.GetPath(), wxT("Error"), wxOK | wxICON_ERROR);
		return;
	}

	wxBitmap bitmap(image);
	m_imageControl->SetBitmap(bitmap);
	Layout();
	FitInside();
	Scroll(0, 0);
}
#include <wx/wx.h>
#include "Scroller.h"
#include "LoadImageEvent.h"

Scroller::Scroller(wxWindow* parent)
	: wxScrolledWindow(parent, wxID_ANY)
{
	SetScrollRate(64, 64);
	auto sizer = new wxBoxSizer(wxVERTICAL);
	m_imageControl = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap);
	sizer->Add(m_imageControl, 1, wxEXPAND, 0);
	SetSizer(sizer);

	Bind(APP_EVT_LOAD_IMAGE, &Scroller::OnLoadImage, this);
}

void Scroller::OnLoadImage(LoadImageEvent& event)
{
	wxImage image;
	auto result = image.LoadFile(event.GetPath());
	if (!result)
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

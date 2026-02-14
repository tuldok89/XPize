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
	wxInitAllImageHandlers();

	Bind(APP_EVT_LOAD_IMAGE, &Scroller::OnLoadImage, this);
}

void Scroller::OnLoadImage(LoadImageEvent& event)
{
	wxImage image;
	image.LoadFile(event.GetPath());
	wxBitmap bitmap(image);
	m_imageControl->SetBitmap(bitmap);
	Layout();
	FitInside();
	Scroll(0, 0);
}

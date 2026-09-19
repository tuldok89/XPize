#include <wx/wx.h>
#include "Scroller.h"
#include "LoadImageEvent.h"

Scroller::Scroller(wxWindow* parent)
	: wxScrolledWindow(parent, wxID_ANY)
{
	SetScrollRate(1, 1);
	auto sizer = new wxBoxSizer(wxVERTICAL);
	m_imageControl = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap);
	sizer->Add(m_imageControl, 0, wxALIGN_CENTER_HORIZONTAL);
	SetSizer(sizer);

	Bind(APP_EVT_LOAD_IMAGE, &Scroller::OnLoadImage, this);
	Bind(wxEVT_MOUSEWHEEL, &Scroller::OnMouseWheel, this);
}

void Scroller::OnLoadImage(LoadImageEvent& event)
{
	wxImage image;
	auto result = image.LoadFile(event.GetPath().Prepend(wxT("\\\\?\\")));
	if (!result)
	{
		wxMessageBox(wxT("Failed to load image: ") + event.GetPath(), wxT("Error"), wxOK | wxICON_ERROR);
		return;
	}

	wxBitmap bitmap(image);
	m_imageControl->SetBitmap(bitmap);

	FitInside();
	Layout();
	Scroll(0, 0);
}

void Scroller::OnMouseWheel(wxMouseEvent& event)
{
	if (event.GetWheelAxis() != wxMOUSE_WHEEL_VERTICAL)
	{
		event.Skip();
		return;
	}

	const int wheelDelta = event.GetWheelDelta();
	const int rotation = event.GetWheelRotation();
	const int maxPos = GetScrollLines(wxVERTICAL) - GetScrollPageSize(wxVERTICAL);
	if (maxPos <= 0 || wheelDelta <= 0 || rotation == 0)
	{
		event.Skip();
		return;
	}

	int posY = 0;
	GetViewStart(nullptr, &posY);

	long delta = static_cast<long>(rotation) * 64 * event.GetLinesPerAction() / wheelDelta;
	long pos = posY - delta;
	pos = wxMin(wxMax(pos, 0L), maxPos);

	if (pos != posY)
	{
		Scroll(0, static_cast<int>(pos));
	}
}

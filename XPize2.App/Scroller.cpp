#include <wx/wx.h>
#include <wx/statbmp.h>

#include "Scroller.h"
#include "LoadImageEvent.h"

BEGIN_EVENT_TABLE(Scroller, wxScrolledWindow)
	EVT_LOAD_IMAGE(Scroller::OnLoadImage)
	EVT_MOUSEWHEEL(Scroller::OnMouseWheel)
	EVT_CHAR(Scroller::OnChar)
END_EVENT_TABLE()

Scroller::Scroller(wxWindow* parent)
	: wxScrolledWindow(parent, wxID_ANY)
{
	SetScrollRate(1, 1);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	m_imageControl = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap);
	sizer->Add(m_imageControl, 0, wxALIGN_CENTER_HORIZONTAL);
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

	FitInside();
	Layout();
	Scroll(0, 0);
}

void Scroller::OnMouseWheel(wxMouseEvent& event)
{
	const int wheelDelta = event.GetWheelDelta();
	const int rotation = event.GetWheelRotation();

	int virtualHeight = 0;
	int clientHeight = 0;
	GetVirtualSize(NULL, &virtualHeight);
	GetClientSize(NULL, &clientHeight);
	const int maxPos = virtualHeight - clientHeight;
	if (maxPos <= 0 || wheelDelta <= 0 || rotation == 0)
	{
		event.Skip();
		return;
	}

	int posX = 0;
	int posY = 0;
	GetViewStart(&posX, &posY);

	long delta = static_cast<long>(rotation) * 64 * event.GetLinesPerAction() / wheelDelta;
	long pos = posY - delta;
	pos = wxMin(wxMax(pos, 0L), maxPos);

	if (pos != posY)
	{
		Scroll(posX, static_cast<int>(pos));
	}
}

void Scroller::OnChar(wxKeyEvent& event)
{
	int virtualWidth = 0;
	int virtualHeight = 0;
	GetVirtualSize(&virtualWidth, &virtualHeight);

	int clientWidth = 0;
	int clientHeight = 0;
	GetClientSize(&clientWidth, &clientHeight);

	const int maxPosX = virtualWidth - clientWidth;
	const int maxPosY = virtualHeight - clientHeight;

	int posX = 0;
	int posY = 0;
	GetViewStart(&posX, &posY);

	long newX = posX;
	long newY = posY;

	switch (event.GetKeyCode())
	{
		case WXK_LEFT:
			newX = posX - 64;
			break;
		case WXK_RIGHT:
			newX = posX + 64;
			break;
		case WXK_UP:
			newY = posY - 64;
			break;
		case WXK_DOWN:
			newY = posY + 64;
			break;
		default:
			event.Skip();
			return;
	}

	newX = wxMin(wxMax(newX, 0L), maxPosX);
	newY = wxMin(wxMax(newY, 0L), maxPosY);

	if (newX != posX || newY != posY)
	{
		Scroll(static_cast<int>(newX), static_cast<int>(newY));
	}
}
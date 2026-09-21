#pragma once
#include <wx/scrolwin.h>

class wxStaticBitmap;
class wxWindow;
class wxMouseEvent;
class wxKeyEvent;
class LoadImageEvent;

class Scroller : public wxScrolledWindow
{
public:
	Scroller(wxWindow* parent);
	void OnLoadImage(LoadImageEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	void OnChar(wxKeyEvent& event);
private:
	wxStaticBitmap* m_imageControl;

	DECLARE_EVENT_TABLE()
};
#pragma once
#include <wx/scrolwin.h>

class wxStaticBitmap;
class wxWindow;
class LoadImageEvent;

class Scroller : public wxScrolledWindow
{
public:
	Scroller(wxWindow* parent);
	void OnLoadImage(LoadImageEvent& event);
private:
	wxStaticBitmap* m_imageControl;

	DECLARE_EVENT_TABLE()
};
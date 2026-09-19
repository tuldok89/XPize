#pragma once

class wxScrolledWindow;
class wxStaticBitmap;
class wxWindow;
class wxMouseEvent;
class LoadImageEvent;

class Scroller : public wxScrolledWindow
{
public:
	Scroller(wxWindow* parent);
	void OnLoadImage(LoadImageEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
private:
	wxStaticBitmap* m_imageControl;
};


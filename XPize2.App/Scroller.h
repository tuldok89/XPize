#pragma once

class wxScrolledWindow;
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
};


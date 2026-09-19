#include <wx/wx.h>
#include "MainApp.h"
#include "AppFrame.h"

bool MainApp::OnInit()
{
	if (!wxApp::OnInit())
	{
		return false;
	}
	wxInitAllImageHandlers();

	AppFrame* window = new AppFrame();
	window->Show(true);

	return true;
}

IMPLEMENT_APP(MainApp)
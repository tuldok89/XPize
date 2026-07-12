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

	auto window = new AppFrame();
	window->Show(true);

	return true;
}

wxIMPLEMENT_APP(MainApp);
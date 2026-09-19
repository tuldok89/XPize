#pragma once
#include <wx/string.h>
#include <wx/event.h>
#include <wx/frame.h>
#include <vector>

class wxCommandEvent;
class wxIcon;
class Scroller;
class StatusBar;
class wxProcess;
class ExtractionDoneEvent;

class AppFrame : public wxFrame
{
public:
	AppFrame();
	virtual ~AppFrame();

	// Called by the extraction process when 7-Zip terminates.
	void OnExtractionProcessEnded();

private:
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnLoadFile(wxCommandEvent& event);
	void OnNextImage(wxCommandEvent& event);
	void OnPreviousImage(wxCommandEvent& event);
	void OnFirstImage(wxCommandEvent& event);
	void OnLastImage(wxCommandEvent& event);
	void OnJumpPage(wxCommandEvent& event);
	void OnExtractionDone(ExtractionDoneEvent& event);
	void OnAboutToolkit(wxCommandEvent& event);
	void LoadPage(size_t index);

	wxIcon m_appIcon;
	Scroller* m_scroller;
	StatusBar* m_statusBar;
	std::vector<wxString> m_currentFileList;
	wxString m_outputPath;
	size_t m_currentIndex;
	wxProcess* m_extractionProcess;

	DECLARE_EVENT_TABLE()
};
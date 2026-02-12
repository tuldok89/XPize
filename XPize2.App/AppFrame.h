#pragma once
#include <wx/string.h>
#include <vector>

class wxFrame;
class wxCommandEvent;
class wxIcon;
class Scroller;
class ExtractionDoneEvent;

class AppFrame : public wxFrame
{
public:
	AppFrame();
	virtual ~AppFrame();

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
	static void ListFilesRecursive(const wxString& path, std::vector<wxString>& out);
	void ListFiles(const wxString& path);
	static bool comparator(const wxString& a, const wxString& b);
	
	wxIcon m_appIcon{};
	Scroller* m_scroller;
	std::vector<wxString> m_currentFileList;
	wxString m_outputPath;
	std::vector<wxString>::const_iterator m_currentFile;
};


#include <wx/wx.h>
#include <wx/aboutdlg.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/mimetype.h>
#include <wx/textdlg.h>

#include <algorithm>
#include <future>
#include <memory>

#include <Shlwapi.h>

#include "AppFrame.h"
#include "Constants.h"
#include "Scroller.h"
#include "LoadImageEvent.h"
#include "Util.h"
#include "ArchiveExtractor.h"
#include "ExtractionDoneEvent.h"

AppFrame::AppFrame() : wxFrame(nullptr, wxID_ANY, wxT("XPize Comic Book Reader"))
{
	auto navMenu = new wxMenu;
	auto nextMenuItem = new wxMenuItem(navMenu, wxID_FORWARD, wxT("Next Image"), wxT("Load next image"));
	nextMenuItem->SetAccel(new wxAcceleratorEntry(wxACCEL_NORMAL, WXK_PAGEDOWN));
	auto prevMenuItem = new wxMenuItem(navMenu, wxID_BACKWARD, wxT("Previous Image"), wxT("Load previous image"));
	prevMenuItem->SetAccel(new wxAcceleratorEntry(wxACCEL_NORMAL, WXK_PAGEUP));
	auto firstMenuItem = new wxMenuItem(navMenu, wxID_FIRST, wxT("First Image"), wxT("Load first image"));
	firstMenuItem->SetAccel(new wxAcceleratorEntry(wxACCEL_NORMAL, WXK_HOME));
	auto lastMenuItem = new wxMenuItem(navMenu, wxID_LAST, wxT("Last Image"), wxT("Load last image"));
	lastMenuItem->SetAccel(new wxAcceleratorEntry(wxACCEL_NORMAL, WXK_END));
	auto jumpMenuItem = new wxMenuItem(navMenu, wxID_JUMP_TO, wxT("Jump to..."), wxT("Jump to page"));
	jumpMenuItem->SetAccel(new wxAcceleratorEntry(wxACCEL_CTRL, static_cast<int>('J')));

	navMenu->Append(nextMenuItem);
	navMenu->Append(prevMenuItem);
	navMenu->Append(firstMenuItem);
	navMenu->Append(lastMenuItem);
	navMenu->Append(jumpMenuItem);

	auto fileMenu = new wxMenu;
	fileMenu->Append(wxID_OPEN);
	fileMenu->AppendSubMenu(navMenu, wxT("Navigation"));
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT);

	auto helpMenu = new wxMenu;
	helpMenu->Append(wxID_ABOUT);

	auto menuBar = new wxMenuBar;
	menuBar->Append(fileMenu, wxT("&File"));
	menuBar->Append(helpMenu, wxT("&Help"));

	m_scroller = new Scroller(this);
	m_appIcon = wxIcon(wxT("IDI_APPICON"), wxBITMAP_TYPE_ICO_RESOURCE);

	SetIcon(m_appIcon);
	SetMenuBar(menuBar);
	CreateStatusBar();
	SetStatusText(wxT("Welcome to XPize Comic Book Reader!"));
	Maximize();

	Bind(wxEVT_MENU, &AppFrame::OnLoadFile, this, wxID_OPEN);
	Bind(wxEVT_MENU, &AppFrame::OnExit, this, wxID_EXIT);
	Bind(wxEVT_MENU, &AppFrame::OnAbout, this, wxID_ABOUT);
	Bind(wxEVT_MENU, &AppFrame::OnNextImage, this, wxID_FORWARD);
	Bind(wxEVT_MENU, &AppFrame::OnPreviousImage, this, wxID_BACKWARD);
	Bind(wxEVT_MENU, &AppFrame::OnFirstImage, this, wxID_FIRST);
	Bind(wxEVT_MENU, &AppFrame::OnLastImage, this, wxID_LAST);
	Bind(wxEVT_MENU, &AppFrame::OnJumpPage, this, wxID_JUMP_TO);
	Bind(APP_EVT_EXTRACTION_DONE, &AppFrame::OnExtractionDone, this);
}

AppFrame::~AppFrame()
{
	wxFileName appTempDir;
	appTempDir.AssignDir(wxStandardPaths::Get().GetTempDir());
	appTempDir.AppendDir(APP_TEMP_FOLDER);
	appTempDir.Rmdir(wxPATH_RMDIR_RECURSIVE);
}

void AppFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void AppFrame::OnAbout(wxCommandEvent& event)
{
	wxAboutDialogInfo info;
	info.SetName(APP_NAME);
	info.SetDescription(APP_DESCRIPTION);
	info.SetCopyright(APP_COPYRIGHT);
	info.AddDeveloper(APP_DEVELOPER);
	info.SetVersion(APP_VERSION);
	info.SetIcon(m_appIcon);
	info.SetWebSite(APP_WEBSITE);

	wxAboutBox(info);
}

void AppFrame::OnLoadFile(wxCommandEvent& event)
{
	wxFileDialog openFileDialog(this,
		wxT("Open Comic Book File"),
		wxEmptyString,
		wxEmptyString,
		wxT("Comic Book Files|*.cbz;*.cbr;*.cbt;*.cb7;*.zip;*.rar;*.tar;*.7z|All files (*.*)|*.*"),
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	auto result = openFileDialog.ShowModal();

	if (result != wxID_OK)
	{
		return;
	}

	auto inputArchive = openFileDialog.GetPath();
	wxFileName inputFileName(inputArchive);
	auto tempFolder = wxStandardPaths::Get().GetTempDir();
	wxFileName outputFolder;
	outputFolder.AssignDir(tempFolder);
	outputFolder.AppendDir(APP_TEMP_FOLDER);
	outputFolder.AppendDir(inputFileName.GetName());
	m_outputPath = outputFolder.GetFullPath();

	this->SetCursor(wxCursor(wxCURSOR_WAIT));

	auto& outputPath = m_outputPath;

	std::future<size_t> extractionTask = std::async(std::launch::async, [this, &inputArchive, outputPath]() {
		ArchiveExtractor extractor;
		auto count = extractor.extract(inputArchive, outputPath);
		auto doneEvent = new ExtractionDoneEvent(APP_EVT_EXTRACTION_DONE, wxID_ANY);
		wxQueueEvent(this, doneEvent);
		return count;
		});

	SetStatusText(inputArchive);
}

void AppFrame::OnNextImage(wxCommandEvent& event)
{
	if (m_currentFile == m_currentFileList.cend() - 1)
		return;
	++m_currentFile;
	wxPostEvent(m_scroller, LoadImageEvent(APP_EVT_LOAD_IMAGE, wxID_ANY, *m_currentFile));
}

void AppFrame::OnPreviousImage(wxCommandEvent& event)
{
	if (m_currentFile == m_currentFileList.cbegin())
		return;
	--m_currentFile;
	wxPostEvent(m_scroller, LoadImageEvent(APP_EVT_LOAD_IMAGE, wxID_ANY, *m_currentFile));
}

void AppFrame::OnFirstImage(wxCommandEvent& event)
{
	if (m_currentFileList.empty())
		return;
	m_currentFile = m_currentFileList.cbegin();
	wxPostEvent(m_scroller, LoadImageEvent(APP_EVT_LOAD_IMAGE, wxID_ANY, *m_currentFile));
}

void AppFrame::OnLastImage(wxCommandEvent& event)
{
	if (m_currentFileList.empty())
		return;
	m_currentFile = m_currentFileList.cend() - 1;
	wxPostEvent(m_scroller, LoadImageEvent(APP_EVT_LOAD_IMAGE, wxID_ANY, *m_currentFile));
}

void AppFrame::OnJumpPage(wxCommandEvent& event)
{
	if (m_currentFileList.empty())
		return;

	wxTextEntryDialog input(this, wxString::Format(wxT("Max value: %s"), std::to_string(m_currentFileList.size())), wxT("Enter page number"));

	if (input.ShowModal() == wxID_OK)
	{
		int value;
		auto result = input.GetValue().ToInt(&value);
		if (!result || value <= 0 || value > static_cast<int>(m_currentFileList.size()))
		{
			wxMessageBox(wxT("Invalid page"), wxT("Input error"), wxOK | wxICON_EXCLAMATION);
			return;
		}

		m_currentFile = m_currentFileList.cbegin() + value - 1;
		wxPostEvent(m_scroller, LoadImageEvent(APP_EVT_LOAD_IMAGE, wxID_ANY, *m_currentFile));
	}

}

void AppFrame::OnExtractionDone(ExtractionDoneEvent& event)
{
	this->SetCursor(wxCursor(wxCURSOR_DEFAULT));
	m_currentFileList.clear();
	ListFiles(m_outputPath);
	std::sort(m_currentFileList.begin(), m_currentFileList.end(), comparator);
	m_currentFile = m_currentFileList.begin();

	if (m_currentFile == m_currentFileList.end())
	{
		wxMessageBox(wxT("No images found in the archive."), APP_NAME, wxOK | wxICON_INFORMATION);
		return;
	}

	wxPostEvent(m_scroller, LoadImageEvent(APP_EVT_LOAD_IMAGE, wxID_ANY, *m_currentFile));
}

void AppFrame::ListFilesRecursive(const wxString& path, std::vector<wxString>& out)
{
	wxDir dir(path);
	if (!dir.IsOpened())
		return;

	wxString name;
	wxMimeTypesManager mgr;

	// Files in this directory
	bool cont = dir.GetFirst(&name, wxEmptyString, wxDIR_FILES);

	while (cont)
	{
		wxFileName filePath;
		filePath.Assign(path);
		filePath.SetFullName(name);

		auto fileType = std::unique_ptr<wxFileType>(mgr.GetFileTypeFromExtension(filePath.GetExt()));
		wxString mimeType;

		fileType->GetMimeType(&mimeType);

		if (fileType != nullptr && fileType->GetMimeType(&mimeType)
			&& mimeType == wxT("image/jpeg") || mimeType == wxT("image/png")
			|| mimeType == wxT("image/gif") || mimeType == wxT("image/bmp")
			|| mimeType == wxT("image/webp") || mimeType == wxT("image/tiff"))
		{
			out.push_back(filePath.GetFullPath());
		}
		
		cont = dir.GetNext(&name);
	}

	// Subdirectories
	cont = dir.GetFirst(&name, wxEmptyString, wxDIR_DIRS);
	while (cont)
	{
		if (name != "." && name != "..")
		{
			wxFileName dirPath;
			dirPath.Assign(path);
			dirPath.AppendDir(name);
			auto sub = dirPath.GetFullPath();
			ListFilesRecursive(sub, out); // recurse
		}
		cont = dir.GetNext(&name);
	}
}

void AppFrame::ListFiles(const wxString& path)
{
	ListFilesRecursive(path, m_currentFileList);
}

bool AppFrame::comparator(const wxString& a, const wxString& b)
{
	return StrCmpLogicalW(a.wc_str(), b.wc_str()) < 0;
}

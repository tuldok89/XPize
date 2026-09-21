#include <wx/wx.h>
#include <wx/aboutdlg.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/textdlg.h>
#include <wx/utils.h>
#include <wx/process.h>

#include <algorithm>

#include "AppFrame.h"
#include "Constants.h"
#include "Scroller.h"
#include "LoadImageEvent.h"
#include "Util.h"
#include "ArchiveExtractor.h"
#include "ExtractionDoneEvent.h"
#include "DirTraverser.h"
#include "NavigationEvent.h"
#include "StatusBar.h"

// Menu command IDs (below wxID_LOWEST so they never clash with the standard
// IDs). wxWidgets 2.8 does not define wxID_FIRST/wxID_LAST/wxID_JUMP_TO.
enum AppEvtId {
	ID_ABOUT_TOOLKIT = 2000
};

enum NavMenuId {
	ID_NAV_FIRST = 2001,
	ID_NAV_LAST,
	ID_NAV_JUMP_TO
};

// wxProcess subclass running the 7-Zip extraction; posts the completion event
// to the frame and destroys itself when the process terminates.
class ExtractionProcess : public wxProcess
{
public:
	ExtractionProcess(AppFrame* frame) : wxProcess(), m_frame(frame) {}

	virtual void OnTerminate(int pid, int status)
	{
		if (m_frame)
		{
			m_frame->OnExtractionProcessEnded();
			ExtractionDoneEvent done(APP_EVT_EXTRACTION_DONE, wxID_ANY);
			wxPostEvent(m_frame, done);
		}
		delete this;
	}

	void DetachFromFrame()
	{
		m_frame = NULL;
	}

private:
	AppFrame* m_frame;
};

BEGIN_EVENT_TABLE(AppFrame, wxFrame)
	EVT_MENU(wxID_OPEN, AppFrame::OnLoadFile)
	EVT_MENU(wxID_EXIT, AppFrame::OnExit)
	EVT_MENU(wxID_ABOUT, AppFrame::OnAbout)
	EVT_MENU(ID_ABOUT_TOOLKIT, AppFrame::OnAboutToolkit)
	EVT_MENU(wxID_FORWARD, AppFrame::OnNextImage)
	EVT_MENU(wxID_BACKWARD, AppFrame::OnPreviousImage)
	EVT_MENU(ID_NAV_FIRST, AppFrame::OnFirstImage)
	EVT_MENU(ID_NAV_LAST, AppFrame::OnLastImage)
	EVT_MENU(ID_NAV_JUMP_TO, AppFrame::OnJumpPage)
	EVT_EXTRACTION_DONE(AppFrame::OnExtractionDone)
END_EVENT_TABLE()

AppFrame::AppFrame() : wxFrame(NULL, wxID_ANY, wxT("XPize Comic Book Reader"))
{
	wxMenu* navMenu = new wxMenu;
	wxMenuItem* nextMenuItem = new wxMenuItem(navMenu, wxID_FORWARD, wxT("&Next Image"), wxT("Load next image"));
	nextMenuItem->SetAccel(new wxAcceleratorEntry(wxACCEL_NORMAL, WXK_PAGEDOWN));
	wxMenuItem* prevMenuItem = new wxMenuItem(navMenu, wxID_BACKWARD, wxT("&Previous Image"), wxT("Load previous image"));
	prevMenuItem->SetAccel(new wxAcceleratorEntry(wxACCEL_NORMAL, WXK_PAGEUP));
	wxMenuItem* firstMenuItem = new wxMenuItem(navMenu, ID_NAV_FIRST, wxT("&First Image"), wxT("Load first image"));
	firstMenuItem->SetAccel(new wxAcceleratorEntry(wxACCEL_NORMAL, WXK_HOME));
	wxMenuItem* lastMenuItem = new wxMenuItem(navMenu, ID_NAV_LAST, wxT("&Last Image"), wxT("Load last image"));
	lastMenuItem->SetAccel(new wxAcceleratorEntry(wxACCEL_NORMAL, WXK_END));
	wxMenuItem* jumpMenuItem = new wxMenuItem(navMenu, ID_NAV_JUMP_TO, wxT("&Jump to..."), wxT("Jump to page"));
	jumpMenuItem->SetAccel(new wxAcceleratorEntry(wxACCEL_CTRL, 'J'));

	navMenu->Append(nextMenuItem);
	navMenu->Append(prevMenuItem);
	navMenu->Append(firstMenuItem);
	navMenu->Append(lastMenuItem);
	navMenu->Append(jumpMenuItem);

	wxMenu* fileMenu = new wxMenu;
	fileMenu->Append(wxID_OPEN);
	fileMenu->AppendSubMenu(navMenu, wxT("&Navigation"));
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT, wxT("&Quit\tCtrl-Q"));

	wxMenu* helpMenu = new wxMenu;
	helpMenu->Append(wxID_ABOUT);
	helpMenu->Append(ID_ABOUT_TOOLKIT, wxT("About Toolkit"));

	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(fileMenu, wxT("&File"));
	menuBar->Append(helpMenu, wxT("&Help"));

	m_scroller = new Scroller(this);
	m_appIcon = wxIcon(wxT("IDI_APPICON"), wxBITMAP_TYPE_ICO_RESOURCE);

	SetIcon(m_appIcon);
	SetMenuBar(menuBar);
	m_statusBar = new StatusBar(wxT("Welcome to XPize Comic Book Reader!"), this);
	SetStatusBar(m_statusBar);
	Maximize();

	m_currentIndex = 0;
	m_extractionProcess = NULL;
}

// Forward declaration (the definition is below). It must appear before the
// destructor, which is the first user of DeleteDirectoryTree.
static void DeleteDirectoryTree(const wxString& path);

AppFrame::~AppFrame()
{
	if (m_extractionProcess)
	{
		ExtractionProcess* process = static_cast<ExtractionProcess*>(m_extractionProcess);
		process->DetachFromFrame();
		m_extractionProcess = NULL;
	}

	wxFileName appTempDir;
	appTempDir.AssignDir(wxStandardPaths::Get().GetTempDir());
	appTempDir.AppendDir(APP_TEMP_FOLDER);

	// wxWidgets 2.8 has no recursive wxFileName::Rmdir and cmd.exe does not
	// exist on Windows 98, so we delete the tree ourselves with the Win32 API.
	DeleteDirectoryTree(appTempDir.GetPath(wxPATH_GET_VOLUME));
}

// ---------------------------------------------------------------------------
// Recursive directory deletion. wxWidgets 2.8 has no recursive
// wxFileName::Rmdir (wxPATH_RMDIR_RECURSIVE only appears in wxWidgets 2.9+)
// and cmd.exe does not exist on Windows 98, so we build on the library-level
// recursion loop that wxWidgets 2.8 does provide: wxDir::Traverse() with a
// wxDirTraverser subclass (the very mechanism this project's own DirTraverser
// and 7-Zip's wx port use). Files are collected as they are found, and
// directories -- reported parent-first by Traverse -- are removed
// deepest-first once the walk completes.
// ---------------------------------------------------------------------------
static void DeleteDirectoryTree(const wxString& path)
{
	class TreeRemovalTraverser : public wxDirTraverser
	{
	public:
		TreeRemovalTraverser() { }

		virtual wxDirTraverseResult OnFile(const wxString& filename)
		{
			m_files.Add(filename);
			return wxDIR_CONTINUE;
		}

		virtual wxDirTraverseResult OnDir(const wxString& dirname)
		{
			// Traverse reports directories parent-first (before descending into
			// them), so we collect them here and remove them deepest-first once
			// the walk has finished, exactly as 7-Zip does.
			m_dirs.Add(dirname);
			return wxDIR_CONTINUE;
		}

		wxArrayString m_files;
		wxArrayString m_dirs;
	};

	TreeRemovalTraverser traverser;
	wxDir dir(path);
	if (!dir.IsOpened())
	{
		// The directory does not exist (or cannot be opened).
		return;
	}

	dir.Traverse(traverser);

	// Delete every file first; clear the read-only attribute exactly as
	// 7-Zip does (the W[ide] entry points are provided on Windows 98 by
	// unicows.dll).
	for (size_t i = 0; i < traverser.m_files.GetCount(); i++)
	{
		::SetFileAttributesW(traverser.m_files[i].c_str(), FILE_ATTRIBUTE_NORMAL);
		::DeleteFileW(traverser.m_files[i].c_str());
	}

	// Traverse reported the directories parent-first, so walk the collected
	// list backwards to remove the deepest directories first. The root
	// directory itself is removed last.
	for (size_t i = traverser.m_dirs.GetCount(); i > 0; i--)
	{
		::RemoveDirectoryW(traverser.m_dirs[i - 1].c_str());
	}
	::RemoveDirectoryW(path.c_str());
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

void AppFrame::OnAboutToolkit(wxCommandEvent& event)
{
	wxMessageBox(wxVERSION_STRING, wxT("About wxWidgets"), wxOK | wxICON_INFORMATION);
}

void AppFrame::OnLoadFile(wxCommandEvent& event)
{
	wxFileDialog openFileDialog(this,
		wxT("Open Comic Book File"),
		wxEmptyString,
		wxEmptyString,
		wxT("Comic Book Files|*.cbz;*.cbr;*.cbt;*.cb7;*.zip;*.rar;*.tar;*.7z|All files (*.*)|*.*"),
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() != wxID_OK)
	{
		return;
	}

	wxString inputArchive = openFileDialog.GetPath();
	wxFileName inputFileName(inputArchive);
	wxString tempFolder = wxStandardPaths::Get().GetTempDir();
	wxFileName outputFolder;
	outputFolder.AssignDir(tempFolder);
	outputFolder.AppendDir(APP_TEMP_FOLDER);
	outputFolder.AppendDir(inputFileName.GetName());
	m_outputPath = outputFolder.GetFullPath();

	this->SetCursor(wxCursor(wxCURSOR_WAIT));

	ExtractionProcess* process = new ExtractionProcess(this);
	ArchiveExtractor extractor;
	if (!extractor.extract(inputArchive, m_outputPath, process))
	{
		delete process;
		this->SetCursor(wxCursor(wxCURSOR_DEFAULT));
		wxMessageBox(wxT("Failed to start the archive extraction tool."), APP_NAME, wxOK | wxICON_ERROR);
		return;
	}
	m_extractionProcess = process;
	m_statusBar->SetStatusText(inputArchive, 0);
}

void AppFrame::OnExtractionProcessEnded()
{
	m_extractionProcess = NULL;
}

void AppFrame::LoadPage(size_t index)
{
	LoadImageEvent evt(APP_EVT_LOAD_IMAGE, wxID_ANY, m_currentFileList[index]);
	wxPostEvent(m_scroller, evt);

	NavigationEvent nav(APP_EVT_NAVIGATION, wxID_ANY, (int)index + 1, (int)m_currentFileList.size());
	wxPostEvent(m_statusBar, nav);
}

void AppFrame::OnNextImage(wxCommandEvent& event)
{
	if (m_currentFileList.empty() || m_currentIndex + 1 >= m_currentFileList.size())
		return;
	++m_currentIndex;
	LoadPage(m_currentIndex);
}

void AppFrame::OnPreviousImage(wxCommandEvent& event)
{
	if (m_currentFileList.empty() || m_currentIndex == 0)
		return;
	--m_currentIndex;
	LoadPage(m_currentIndex);
}

void AppFrame::OnFirstImage(wxCommandEvent& event)
{
	if (m_currentFileList.empty())
		return;
	m_currentIndex = 0;
	LoadPage(m_currentIndex);
}

void AppFrame::OnLastImage(wxCommandEvent& event)
{
	if (m_currentFileList.empty())
		return;
	m_currentIndex = m_currentFileList.size() - 1;
	LoadPage(m_currentIndex);
}

void AppFrame::OnJumpPage(wxCommandEvent& event)
{
	if (m_currentFileList.empty())
		return;

	wxString message;
	message.Printf(wxT("Total pages: %u"), (unsigned)m_currentFileList.size());
	wxTextEntryDialog input(this, message, wxT("Enter page number"));

	if (input.ShowModal() == wxID_OK)
	{
		long value;
		if (!input.GetValue().ToLong(&value) || value <= 0 || value > (int)m_currentFileList.size())
		{
			wxMessageBox(wxT("Invalid page"), wxT("Input error"), wxOK | wxICON_EXCLAMATION);
			return;
		}
		m_currentIndex = (size_t)value - 1;
		LoadPage(m_currentIndex);
	}
}

void AppFrame::OnExtractionDone(ExtractionDoneEvent& event)
{
	m_currentFileList.clear();
	DirTraverser traverser(m_currentFileList);
	wxDir dir(m_outputPath);

	this->SetCursor(wxCursor(wxCURSOR_DEFAULT));
	dir.Traverse(traverser);
	std::sort(m_currentFileList.begin(), m_currentFileList.end(), NaturalComparer());
	m_currentIndex = 0;

	if (m_currentFileList.empty())
	{
		wxMessageBox(wxT("No images found in the archive."), APP_NAME, wxOK | wxICON_INFORMATION);
		return;
	}
	LoadPage(m_currentIndex);
}
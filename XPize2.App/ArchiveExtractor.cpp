#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/process.h>
#include <wx/utils.h>

#include "ArchiveExtractor.h"

#ifndef XPIZE2_7ZIP_CLI
#error "XPIZE2_7ZIP_CLI compile definition is required (set the CMake variable XPIZE2_7ZIP_CLI)."
#endif

bool ArchiveExtractor::extract(const wxString& fileName, const wxString& outputDir, wxProcess* handler) const
{
	wxFileName::Mkdir(outputDir, 0777, wxPATH_MKDIR_FULL);

	wxString command;
	command.Printf(wxT("\"%s\" x -y -o\"%s\" \"%s\""),
		wxT(XPIZE2_7ZIP_CLI), outputDir.c_str(), fileName.c_str());

	return wxExecute(command, wxEXEC_ASYNC, handler) != 0;
}
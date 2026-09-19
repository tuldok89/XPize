#pragma once
#include <wx/string.h>

class wxProcess;

class ArchiveExtractor
{
public:
	ArchiveExtractor() {}

	// Launch 7-Zip in the background to extract fileName into outputDir.
	// handler->OnTerminate() is invoked when the process ends; returns true
	// if the process was started successfully.
	bool extract(const wxString& fileName, const wxString& outputDir, wxProcess* handler) const;
};
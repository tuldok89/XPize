#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/mimetype.h>
#include "DirTraverser.h"

DirTraverser::DirTraverser(std::vector<wxString>& files) : m_files(files)
{
}

wxDirTraverseResult DirTraverser::OnFile(const wxString& fileName)
{
	wxFileName filePath;
	wxMimeTypesManager mgr;
	filePath.Assign(fileName);
	
	auto fileType = std::unique_ptr<wxFileType>(mgr.GetFileTypeFromExtension(filePath.GetExt()));
	wxString mimeType;
	fileType->GetMimeType(&mimeType);

	if (fileType != nullptr && fileType->GetMimeType(&mimeType)
		&& mimeType == wxT("image/jpeg") || mimeType == wxT("image/png")
		|| mimeType == wxT("image/gif") || mimeType == wxT("image/bmp")
		|| mimeType == wxT("image/webp") || mimeType == wxT("image/tiff"))
	{
		m_files.push_back(filePath.GetFullPath());
	}

	return wxDIR_CONTINUE;
}

wxDirTraverseResult DirTraverser::OnDir(const wxString&)
{
	return wxDIR_CONTINUE;
}

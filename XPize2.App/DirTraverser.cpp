#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/mimetype.h>

#include <memory>
#include <algorithm>

#include "DirTraverser.h"

wxMimeTypesManager DirTraverser::m_mgr;
const std::array<wxString, 6> DirTraverser::m_allowed = { wxT("image/jpeg"), wxT("image/png"), wxT("image/gif"),
    wxT("image/bmp"), wxT("image/webp"), wxT("image/tiff") };

DirTraverser::DirTraverser(std::vector<wxString>& files) : m_files(files)
{
}

wxDirTraverseResult DirTraverser::OnFile(const wxString& fileName)
{
	wxFileName filePath;
	filePath.Assign(fileName);

	auto fileType = std::unique_ptr<wxFileType>(m_mgr.GetFileTypeFromExtension(filePath.GetExt()));

	if (fileType == nullptr)
	{
		return wxDIR_CONTINUE;
	}

	wxString mimeType;
	auto mimeResult = fileType->GetMimeType(&mimeType);
	auto exists = std::find(m_allowed.cbegin(), m_allowed.cend(), mimeType) != m_allowed.cend();

	if (mimeResult && exists)
	{
		m_files.push_back(filePath.GetFullPath());
	}

	return wxDIR_CONTINUE;
}

wxDirTraverseResult DirTraverser::OnDir(const wxString&)
{
	return wxDIR_CONTINUE;
}

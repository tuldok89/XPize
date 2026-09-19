#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/mimetype.h>

#include "DirTraverser.h"

wxMimeTypesManager DirTraverser::m_mgr;

const wxString DirTraverser::m_allowed[6] = {
	wxT("image/jpeg"), wxT("image/png"), wxT("image/gif"),
	wxT("image/bmp"), wxT("image/webp"), wxT("image/tiff")
};

DirTraverser::DirTraverser(std::vector<wxString>& files) : m_files(files)
{
}

wxDirTraverseResult DirTraverser::OnFile(const wxString& fileName)
{
	wxFileName filePath;
	filePath.Assign(fileName);

	// The pointer is owned by the caller; wxWidgets 2.8 has no
	// wxFileTypePtr.
	wxFileType* fileType = m_mgr.GetFileTypeFromExtension(filePath.GetExt());
	if (fileType == NULL)
	{
		return wxDIR_CONTINUE;
	}

	wxString mimeType;
	bool mimeResult = fileType->GetMimeType(&mimeType);
	delete fileType;

	if (mimeResult)
	{
		for (size_t i = 0; i < sizeof(m_allowed) / sizeof(m_allowed[0]); ++i)
		{
			if (mimeType == m_allowed[i])
			{
				m_files.push_back(filePath.GetFullPath());
				break;
			}
		}
	}

	return wxDIR_CONTINUE;
}

wxDirTraverseResult DirTraverser::OnDir(const wxString&)
{
	return wxDIR_CONTINUE;
}
#pragma once

#include <array>
#include <vector>

#include <wx/dir.h>
#include <wx/string.h>
#include <wx/mimetype.h>

class DirTraverser : public wxDirTraverser
{
public:
	DirTraverser(std::vector<wxString>& files);
	virtual wxDirTraverseResult OnFile(const wxString& fileName) override;
	virtual wxDirTraverseResult OnDir(const wxString&) override;
private:
	std::vector<wxString>& m_files;
    static wxMimeTypesManager m_mgr;
    static const std::array<wxString, 6> m_allowed;
};


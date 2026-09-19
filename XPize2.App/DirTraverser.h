#pragma once
#include <vector>
#include <wx/dir.h>
#include <wx/string.h>
#include <wx/mimetype.h>

class DirTraverser : public wxDirTraverser
{
public:
	DirTraverser(std::vector<wxString>& files);
	virtual wxDirTraverseResult OnFile(const wxString& fileName);
	virtual wxDirTraverseResult OnDir(const wxString& dirname);
private:
	std::vector<wxString>& m_files;
	static wxMimeTypesManager m_mgr;
	static const wxString m_allowed[6];
};
#pragma once
#include <vector>

class wxDirTraverser;
class wxString;

class DirTraverser : public wxDirTraverser
{
public:
	DirTraverser(std::vector<wxString>& files);
	virtual wxDirTraverseResult OnFile(const wxString& fileName) override;
	virtual wxDirTraverseResult OnDir(const wxString&) override;
private:
	std::vector<wxString>& m_files;
};


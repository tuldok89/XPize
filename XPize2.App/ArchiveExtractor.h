#pragma once
#include <wx/string.h>
#include <cstdint>
struct archive;
class ArchiveExtractor
{
public:
	ArchiveExtractor() = default;
	size_t extract(const wxString& fileName, const wxString& outputDir);
private:
	void copyData(struct archive* ar, struct archive* aw);
};


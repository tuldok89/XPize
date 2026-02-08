#pragma once

#include <wx/string.h>
#include <Shlwapi.h>

struct NaturalComparer
{
	bool operator()(const wxString& a, const wxString& b) const
	{
		return StrCmpLogicalW(a.c_str(), b.c_str()) < 0;
	}
};
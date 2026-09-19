#pragma once
#include <wx/string.h>
#include <windows.h>

// StrCmpLogicalW only exists on Windows XP and later; load it dynamically so
// the application keeps working on Windows 98, falling back to a plain
// case-insensitive comparison.
typedef int (CALLBACK * LpStrCmpLogicalW)(LPCWSTR, LPCWSTR);

inline int CompareLogical(const wxString& a, const wxString& b)
{
	static LpStrCmpLogicalW fStrCmpLogicalW = NULL;

	if (fStrCmpLogicalW == NULL)
	{
		HMODULE shlwapi = ::GetModuleHandle(wxT("shlwapi.dll"));
		if (shlwapi)
		{
			fStrCmpLogicalW = (LpStrCmpLogicalW)::GetProcAddress(shlwapi, "StrCmpLogicalW");
		}
	}

	if (fStrCmpLogicalW)
	{
		return fStrCmpLogicalW(a.c_str(), b.c_str());
	}
	return wxStricmp(a, b);
}

struct NaturalComparer
{
	bool operator()(const wxString& a, const wxString& b) const
	{
		return CompareLogical(a, b) < 0;
	}
};
#pragma once

#include <wx/event.h>

class ExtractionDoneEvent : public wxEvent
{
public:
	ExtractionDoneEvent(wxEventType eventType, int winid);
	virtual wxEvent* Clone() const override { return new ExtractionDoneEvent(*this); }
};

wxDECLARE_EVENT(APP_EVT_EXTRACTION_DONE, ExtractionDoneEvent);
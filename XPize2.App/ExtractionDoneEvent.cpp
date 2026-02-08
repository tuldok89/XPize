#include "ExtractionDoneEvent.h"

ExtractionDoneEvent::ExtractionDoneEvent(wxEventType eventType, int winid) : wxEvent(winid, eventType)
{
}

wxDEFINE_EVENT(APP_EVT_EXTRACTION_DONE, ExtractionDoneEvent);
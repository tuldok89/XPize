#include "ExtractionDoneEvent.h"

ExtractionDoneEvent::ExtractionDoneEvent(wxEventType eventType, int winid) : wxEvent(winid, eventType)
{
}

DEFINE_LOCAL_EVENT_TYPE(APP_EVT_EXTRACTION_DONE)
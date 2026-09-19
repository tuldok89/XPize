#include <wx/event.h>

DECLARE_LOCAL_EVENT_TYPE(APP_EVT_EXTRACTION_DONE, 3001)

// See the note in LoadImageEvent.h about wx 2.8 EVT_CUSTOM.
#define EVT_EXTRACTION_DONE(fn) \
	DECLARE_EVENT_TABLE_ENTRY(APP_EVT_EXTRACTION_DONE, wxID_ANY, wxID_ANY, \
		(wxObjectEventFunction) (wxEventFunction) &fn, NULL),

class ExtractionDoneEvent : public wxEvent
{
public:
	ExtractionDoneEvent(wxEventType eventType, int winid);
	virtual wxEvent* Clone() const { return new ExtractionDoneEvent(*this); }
};
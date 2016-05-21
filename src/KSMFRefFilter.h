//	File: KSMFRefFilter.h

#include <FilePanel.h>

class KSMFRefFilter : public BRefFilter
{
public:
	virtual bool Filter(const entry_ref*, BNode*, struct stat*, const char* mimetype);
};


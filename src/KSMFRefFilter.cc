//	File: KSMFRefFilter.cc

#include "KSMFRefFilter.h"

//	System Headers
#include <string.h>
//#include <Debug.h>
#include <Mime.h>

//	Project Headers
#include "SMFProducer.h"

//	libKT Headers
#include "KDebug.h"


bool
KSMFRefFilter::Filter(const entry_ref* ref, BNode* node, struct stat*, const char* mimetype)
{
//	K_DEBUG_MARKER("KSMFRefFilter::Filter()");

	bool aResult = false;

	if (node->IsFile()) {
		BMimeType aMimeType;
		status_t aErr = B_OK;

		aErr = BMimeType::GuessMimeType(ref, &aMimeType);
		if (aErr != B_OK)
			return false;

		if (aMimeType == kMidiType)
			aResult = true;
	} else if (node->IsDirectory()) {
		aResult = true;
	} else if (node->IsSymLink()) {
		BEntry aEntry(ref, true);

		if (aEntry.Exists()) {
			entry_ref aRef;
			aEntry.GetRef(&aRef);

			BNode aNode(&aEntry);

			aResult = Filter(&aRef, &aNode, NULL, NULL);
		}
/*
		BMimeType aMimeType;
		status_t aErr = B_OK;

		aErr = BMimeType::GuessMimeType(aRef, &aMimeType);
		if (aErr != B_OK)
			return false;

		if ((aMimeType == kMidiType) || (aMimeType == "application/x-vnd.be-directory"))
			return true;

//		return true;
*/
	}

	return aResult;
}

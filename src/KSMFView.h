//	File: KSMFView.h

#ifndef __KSMFView_h__
#define __KSMFView_h__

#include <Box.h>

class KSMFView
  : public BBox
{
  public:
	KSMFView(BRect frame, const char* name = NULL, uint32 resizeFlags = B_FOLLOW_ALL);

	virtual void AttachedToWindow();
	virtual void GetPreferredSize(float* width, float* height);

  protected:

  private:
	KSMFView();	//	Not impremented
	KSMFView(const KSMFView&);	//	Not impremented
	KSMFView& operator=(const KSMFView&);	//	Not impremented
};

#endif

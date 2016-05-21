//	File: KSMFView.cc

#include "KSMFView.h"


KSMFView::KSMFView(BRect frame, const char* name, uint32 resizeFlags)
  : BBox(frame, name, resizeFlags)
{
}


void
KSMFView::AttachedToWindow()
{
	BBox::AttachedToWindow();
}


void
KSMFView::GetPreferredSize(float* width, float* height)
{
	BBox::GetPreferredSize(width, height);
}

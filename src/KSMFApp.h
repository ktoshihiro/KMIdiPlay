//======================================================================
//	File: KSMFApp.h
//----------------------------------------------------------------------
//	Copyright © 1999 KISAKA Toshihiro.
//	All rights reserved.
//		noah@gopherwood.com
//		kisaka@pochi.iijnet.or.jp
//======================================================================

#ifndef __KSMFApp_h__
#define __KSMFApp_h__

#include <Application.h>

//	System Headers
#include <memory>

//	Forward Declaration
class BFilePanel;
class BResourceStrings;
class KSMFWindow;


//======================================================================
#pragma mark === class KSMFApp ===
//======================================================================

class KSMFApp
  : public BApplication
{
  public:
	KSMFApp(const char* signature);
	virtual ~KSMFApp();

#if DEBUG
	virtual void AppActivated(bool active);
#endif
	virtual void MessageReceived(BMessage* msg);
	virtual void ReadyToRun();
	virtual void RefsReceived(BMessage* msg);

	const char* FindString(int32 id);

  protected:
  private:
	KSMFApp();	//	Not impremented
	KSMFApp(const KSMFApp&);	//	Not impremented
	KSMFApp& operator=(const KSMFApp&);	//	Not impremented

#if !MULTI_WINDOW
	KSMFWindow* mWindow;
#endif

	auto_ptr<BFilePanel> mFilePanel;
	auto_ptr<BResourceStrings> mStrings;
};

extern KSMFApp* gApp;

#endif

//======================================================================
//	File: KSMFApp.cc
//----------------------------------------------------------------------
//	Copyright © 1999 KISAKA Toshihiro.
//	All rights reserved.
//		noah@gopherwood.com
//		kisaka@pochi.iijnet.or.jp
//======================================================================

#include "KSMFApp.h"

//	System Headers
#include <iostream>
#include <FilePanel.h>
#include <ResourceStrings.h>
#include <Roster.h>
#include <Window.h>

//	Project Headers
#include "KSMFRefFilter.h"
#include "KSMFWindow.h"
#include "KSMFProducer.h"

//	libKT Headers
#include "KDebug.h"
#include "KUtility.h"
#include "KMidiDefs.h"
#include "KMidiRosterAgent.h"


//======================================================================
#pragma mark === class KSMFApp ===
//======================================================================

KSMFApp* gApp = NULL;

//----------------------------------------------------------------------

//----------------------------------------------------------------------

KSMFApp::KSMFApp(const char* signature)
  : BApplication(signature),
#if !MULTI_WINDOW
	mWindow(NULL),
#endif
	mFilePanel(new BFilePanel(B_OPEN_PANEL, NULL, NULL, B_FILE_NODE | B_DIRECTORY_NODE | B_SYMLINK_NODE, true, NULL, new KSMFRefFilter, false, true)),
	mStrings(new BResourceStrings)
{
	gApp = this;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

KSMFApp::~KSMFApp()
{
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

#if DEBUG
void
KSMFApp::AppActivated(bool active)
{
	BMessage* aMessage = CurrentMessage();
	if (aMessage != NULL)
		PRINT_OBJECT((*aMessage));

	BApplication::AppActivated(active);
}
#endif


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KSMFApp::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
	  case K_OPEN_REQUESTED:
		mFilePanel->Show();
		break;
#if 0
	  case B_SILENT_RELAUNCH:
		cout << "B_SILENT_RELAUNCH" << endl;
		break;
	  case B_SOME_APP_LAUNCHED:
		cout << "B_SOME_APP_LAUNCHED" << endl;
		break;
	  case B_SOME_APP_QUIT:
		cout << "B_SOME_APP_QUIT" << endl;
		break;
	  case B_SOME_APP_ACTIVATED:
		{
			cout << "B_SOME_APP_ACTIVATED" << endl;
			BMessage* aMessage = CurrentMessage();
			if (aMessage != NULL)
				PRINT_OBJECT((*aMessage));
		}
		break;
#endif
	  default:
//		PRINT_OBJECT((*msg));
		BApplication::MessageReceived(msg);
		break;
	}
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KSMFApp::ReadyToRun()
{
	status_t aErr = B_OK;

	aErr = KMidiRosterAgent::InitMidiRosterAgent();
	if (aErr != B_OK)
		PRINT(("ERROR: InitMidiRosterAgent failed. (%s)\n", strerror(aErr)));

#if MULTI_WINDOW
	if (CountWindows() == 0) {
		BWindow* aWindow = NULL;

		aWindow = new KSMFWindow(BRect(0, 0, 150, 100).OffsetByCopy(KUtility::StandardWindowLocation()));
		aWindow->Show();
	}
	aWindow->UpdateMenus();
#else
	if (mWindow == NULL) {
		mWindow = new KSMFWindow(BRect(0, 0, 150, 100).OffsetByCopy(KUtility::StandardWindowLocation()));
		mWindow->Show();
	}
	mWindow->UpdateMenus();
#endif
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KSMFApp::RefsReceived(BMessage* msg)
{
#if DEBUG
			fprintf(stderr, "KSMFApp::RefsReceived()\n");
			fflush(stderr);
#endif

	if (msg == NULL)
		return;

#if MULTI_WINDOW
	BWindow* aWindow = NULL;

	aWindow = new KSMFWindow(BRect(0, 0, 150, 100).OffsetByCopy(KUtility::StandardWindowLocation()));
	aWindow->Show();

	msg->what = B_SIMPLE_DATA;
	aWindow->PostMessage(msg);
#else
	if (mWindow == NULL) {
		mWindow = new KSMFWindow(BRect(0, 0, 150, 100).OffsetByCopy(KUtility::StandardWindowLocation()));
		mWindow->Show();
	}

	msg->what = B_SIMPLE_DATA;
	mWindow->PostMessage(msg);

	mWindow->Activate(true);
#endif
}


const char*
KSMFApp::FindString(int32 id)
{
	const char* aString = mStrings->FindString(id);

	if (aString != NULL)
		return aString;
	else
		return "<NULL>";
}

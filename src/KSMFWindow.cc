//======================================================================
//	File: KSMFWindow.cc
//----------------------------------------------------------------------
//	Copyright © 1999 KISAKA Toshihiro.
//	All rights reserved.
//		noah@gopherwood.com
//		kisaka@pochi.iijnet.or.jp
//======================================================================

#include "KSMFWindow.h"

#define DO_PRELUDE 0

//	System Headers
#include <iostream>
#include <algorithm>
#include <Alert.h>
#include <Autolock.h>
#include <Application.h>
#include <Bitmap.h>
#include <Box.h>
//#include <Debug.h>
#include <Directory.h>
#include <Entry.h>
#include <List.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <Message.h>
#include <MidiConsumer.h>
#include <MidiEndpoint.h>
#include <MidiRoster.h>
#include <Mime.h>
#include <Path.h>
#include <PopUpMenu.h>
#include <RecentItems.h>
#include <StringView.h>

//	Project Headers
#include "resource_id.h"
#include "SMFProducer.h"
#include "KSMFApp.h"
#include "KSMFProducer.h"
#include "KMidiUtils.h"
#include "KIconMenuItem.h"

//	libKT Headers
#include "KDebug.h"
//#include "KMidiIcons.h"
#include "KMidiDefs.h"
#include "KMidiRosterAgent.h"


//======================================================================
#pragma mark === class KSMFWindow ===
//======================================================================

//----------------------------------------------------------------------

//----------------------------------------------------------------------

KSMFWindow::KSMFWindow(BRect frame)
  : BWindow(frame, kAppName, B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_ASYNCHRONOUS_CONTROLS | B_QUIT_ON_WINDOW_CLOSE, B_CURRENT_WORKSPACE),
	mSMFProducer(new KSMFProducer(kAppName)),
	mFileIndex(-1),
	mStatus(K_STOP),
	mMenuLock(true)
{
	K_DEBUG_MARKER("KSMFWindow::KSMFWindow()");

	BMenuBar* aMenuBar = CreateMenuBar();
	AddChild(aMenuBar);

	BRect aBounds(Bounds());
	aBounds.top += aMenuBar->Frame().Height() + 1;

	BView* aBaseView = new BView(aBounds, "SMF Test View", B_FOLLOW_ALL, B_WILL_DRAW | B_PULSE_NEEDED);
	AddChild(aBaseView);
	aBaseView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	aBaseView->SetFont(be_fixed_font);

	font_height aHeight;
	be_fixed_font->GetHeight(&aHeight);
	float aMargin = (aHeight.ascent + aHeight.descent + aHeight.leading) * 1.5;

	aBounds = aBaseView->Bounds();
	aBounds.InsetBy(aMargin / 2, aMargin / 2);
	BBox* aBox = new BBox(aBounds, B_EMPTY_STRING, B_FOLLOW_LEFT_RIGHT);
	aBaseView->AddChild(aBox);
	aBox->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));


	BMenuField* aMenuField = NULL;

	aMenuField = new BMenuField(BRect(0, 0, 100, 100), "Files", B_EMPTY_STRING, CreateFilesMenu());
	aBox->SetLabel(aMenuField);
	aMenuField->SetDivider(0.0f);
	UpdateFilesPopUpMenu();
	aMenuField->ResizeToPreferred();

	float aBasePosition = aMargin / 2.0f;

	BStringView* aStringView = NULL;

	aStringView = new BStringView(BRect(aMargin, aBasePosition + aMargin * 1, aMargin, aBasePosition + aMargin * 2), "Format", "Format: ");
	aStringView->SetFont(be_fixed_font);
	aBox->AddChild(aStringView);
	aStringView->ResizeToPreferred();

	aStringView = new BStringView(BRect(aMargin, aBasePosition + aMargin * 2, aMargin, aBasePosition + aMargin * 2), "Score", "Score : ");
	aStringView->SetFont(be_fixed_font);
	aBox->AddChild(aStringView);
	aStringView->ResizeToPreferred();

	aStringView = new BStringView(BRect(aMargin, aBasePosition + aMargin * 3, aMargin, aBasePosition + aMargin * 2), "Tracks", "Tracks: ");
	aStringView->SetFont(be_fixed_font);
	aBox->AddChild(aStringView);
	aStringView->ResizeToPreferred();

	aStringView = new BStringView(BRect(aMargin, aBasePosition + aMargin * 4, aMargin, aBasePosition + aMargin * 2), "Length", "Length: ");
	aStringView->SetFont(be_fixed_font);
	aBox->AddChild(aStringView);
	aStringView->ResizeToPreferred();

	aBounds = aBox->Bounds();
	aBounds.bottom = aStringView->Frame().bottom + aMargin;

	aBox->ResizeTo(aBounds.Width(), aBounds.Height());

	aBounds = Bounds();
	aBounds.bottom = KeyMenuBar()->Bounds().Height() + aBox->Frame().bottom + aMargin / 2;
//	aBounds.right = be_fixed_font->StringWidth("Format: Format 0") + aMargin * 3;
	aBounds.right = be_fixed_font->StringWidth("Score : GM GM2 GS 88 XG") + aMargin * 3;

	ResizeTo(aBounds.Width(), aBounds.Height());

//	mConsumer->SetProducer(mSMFProducer);
//	mConsumer->Register();

//	mSMFProducer->Register();
	PostMessage(K_MIDI_REGISTER);

//	rgb_color aColor;
//	aColor = ui_color(B_PANEL_BACKGROUND_COLOR);
//	cout << "B_PANEL_BACKGROUND_COLOR: " << (int)aColor.red << ", " << (int)aColor.green << ", " << (int)aColor.blue << endl;
//	aColor = ui_color(B_MENU_BACKGROUND_COLOR);
//	cout << "B_PANEL_BACKGROUND_COLOR: " << (int)aColor.red << ", " << (int)aColor.green << ", " << (int)aColor.blue << endl;
//
//	mSMFProducer->Register();

	{
		float aHmin;
		float aHmax;
		float aVmin;
		float aVmax;
		GetSizeLimits(&aHmin, &aHmax, &aVmin, &aVmax);
		PRINT(("aHmin, aHmax, aVmin, aVmax) = (%f, %f, %f, %f)\n", aHmin, aHmax, aVmin, aVmax));
		BMenuBar* aMenuBar = KeyMenuBar();
		aHmin = aMenuBar->ItemAt(aMenuBar->CountItems() - 1)->Frame().right;
		SetSizeLimits(aHmin, aHmax, aVmin, aVmax);
		PRINT(("aHmin, aHmax, aVmin, aVmax) = (%f, %f, %f, %f)\n", aHmin, aHmax, aVmin, aVmax));
	}


	KMidiRosterAgent* aAgent = KMidiRosterAgent::MidiRosterAgent();
	if (aAgent != NULL) {
		StartWatching(aAgent, LIBKT_MIDI_CONNECTED);
		StartWatching(aAgent, LIBKT_MIDI_DISCONNECTED);
	}
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

KSMFWindow::~KSMFWindow()
{
	K_DEBUG_MARKER("KSMFWindow::~KSMFWindow()");

	if (mSMFProducer == NULL)
		return;

	if (mSMFProducer->IsRunning())
		mSMFProducer->Stop();

	mSMFProducer->Release();
//	mConsumer->Release();
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KSMFWindow::MenusBeginning()
{
	K_DEBUG_MARKER("KSMFWindow::MenusBeginning()");

//	mMenuLock.Lock();

	UpdateMenus();
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KSMFWindow::MenusEnded()
{
	K_DEBUG_MARKER("KSMFWindow::MenusEnded()");

//	mMenuLock.Unlock();
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KSMFWindow::MessageReceived(BMessage* msg)
{
	status_t aErr = B_OK;

	switch (msg->what) {
	  case B_OBSERVER_NOTICE_CHANGE:
		{
			PRINT_OBJECT((*msg));

			int32 aChange;

			aErr = msg->FindInt32(B_OBSERVE_WHAT_CHANGE, &aChange);
			if (aErr != B_OK)
				break;

			switch (aChange) {
			  case LIBKT_MIDI_CONNECTED:
			  case LIBKT_MIDI_DISCONNECTED:
				{
					KMidiRosterAgent* aAgent = KMidiRosterAgent::MidiRosterAgent();
					if (aAgent == NULL)
						break;

PRINT(("***** mSMFProducer->ID() = %ld\n", mSMFProducer->ID()));
PRINT(("***** aAgent->CountConnections(mSMFProducer->ID()) = %ld\n", aAgent->CountConnections(mSMFProducer->ID())));
					if (aAgent->CountConnections(mSMFProducer->ID()) == 0)
						Stop();

					UpdateControlMenu();
				}
			  default:
				break;
			}
		}
		break;

	  case B_SIMPLE_DATA:
	  case B_REFS_RECEIVED:
		{
			K_DEBUG_MARKER("KSMFWindow::MessageReceived() -- B_REFS_RECEIVED");

			RefsReceived(msg);
		}
		break;

//	  case B_MIDI_EVENT:
//		{
//			K_DEBUG_MARKER("KSMFWindow::MessageReceived() -- B_MIDI_EVENT");
//
//			HandleMidiEvent(msg);
//		}
//		break;
//
	  case K_SMF_LOAD:
		{
			K_DEBUG_MARKER("KSMFWindow::MessageReceived() -- K_SMF_LOAD");

			status_t aErr = B_OK;
			int32 aIndex = -1;

			aErr = msg->FindInt32("index", &aIndex);
			if (aErr != B_OK)
				break;

			if ((aIndex < 0) || (mFiles.size() <= aIndex))
				break;

			if (aIndex == mFileIndex)
				break;

			mFileIndex = aIndex;

			aErr = Load();
			if (aErr != B_OK)
				mStatus = K_STOP;
			if (mStatus == K_PLAY)
				Start();

			UpdateFilesPopUpMenu();
			UpdateControlMenu();
		}
		break;		

	  case K_MIDI_START:
		{
			K_DEBUG_MARKER("KSMFWindow::MessageReceived() -- K_MIDI_START");

			mStatus = K_PLAY;
			Start();
			UpdateControlMenu();
		}
		break;

	  case K_MIDI_STOP:
		{
			K_DEBUG_MARKER("KSMFWindow::MessageReceived() -- K_MIDI_STOP");

			mStatus = K_STOP;
			Stop();
			UpdateControlMenu();
		}
		break;

	  case K_MIDI_REGISTER:
		{
			K_DEBUG_MARKER("KSMFWindow::MessageReceived() -- K_MIDI_REGISTER");

			BMenuItem* aItem = KeyMenuBar()->FindItem(K_MIDI_REGISTER);

			if (aItem == NULL)
				break;

			if (aItem->IsMarked()) {
				mSMFProducer->Unregister();
				aItem->SetMarked(false);
			} else {
				mSMFProducer->Register();
				aItem->SetMarked(true);
			}
		}
		break;

//	  case K_MIDI_CONNECT:
//		{
//			K_DEBUG_MARKER("KSMFWindow::MessageReceived() -- K_MIDI_CONNECT");
//			PRINT_OBJECT((*msg));
//
//			status_t aErr = B_OK;
//
//			int32 aID = 0;
//			aErr = msg->FindInt32("be:id", &aID);
//			if (aErr != B_OK)
//				break;
//
//			BString aType;
//			aErr = msg->FindString("be:type", &aType);
//			if (aErr != B_OK)
//				break;
//
//			KMidiRosterAgent* aAgent = KMidiRosterAgent::MidiRosterAgent();
//			if (aAgent == NULL)
//				break;
//
////			if (aType == "producer") {
////				aAgent->Connect(aID, mDMP11->ConsumerID());
////#if DO_PRELUDE
//////				KMidiUtils::Prelude(aID);
////#endif
////			}
//			if (aType == "consumer") {
//				aAgent->Connect(mSMFProducer->ID(), aID);
//#if DO_PRELUDE
////				KMidiUtils::Prelude(aID);
//#endif
//			}
////
////#if 1
////			KMidiRosterAgent* aAgent = KMidiRosterAgent::MidiRosterAgent();
////			if (aAgent == NULL)
////				break;
////
////			if (aAgent->IsConnected(mSMFProducer->ID(), aID)) {
////				aAgent->Disconnect(mSMFProducer->ID(), aID);
////				KMidiUtils::Postlude(aID);
////			} else {
////				aAgent->Connect(mSMFProducer->ID(), aID);
////#if DO_PRELUDE
////				KMidiUtils::Prelude(aID);
////#endif
////			}
////#else
////			BMidiConsumer* aConsumer = BMidiRoster::FindConsumer(aID);
////			if (aConsumer == NULL)
////				break;
////
////			if (mSMFProducer->IsConnected(aConsumer)) {
////				mSMFProducer->Disconnect(aConsumer);
////				if (mSMFProducer->Connections()->IsEmpty())
////					Stop();
////
////				KMidiUtils::Postlude(aConsumer);
////			} else {
////#if DO_PRELUDE
////				KMidiUtils::Prelude(aConsumer);
////#endif
////				mSMFProducer->Connect(aConsumer);
////			}
////
////			aConsumer->Release();
////#endif
//			UpdateControlMenu();
//		}
//		break;
//
//	  case K_MIDI_DISCONNECT:
//		{
//			K_DEBUG_MARKER("KSMFWindow::MessageReceived() -- K_MIDI_CONNECT");
//			PRINT_OBJECT((*msg));
//
//			status_t aErr = B_OK;
//
//			int32 aID = 0;
//			aErr = msg->FindInt32("be:id", &aID);
//			if (aErr != B_OK)
//				break;
//
//			BString aType;
//			aErr = msg->FindString("be:type", &aType);
//			if (aErr != B_OK)
//				break;
//
//			KMidiRosterAgent* aAgent = KMidiRosterAgent::MidiRosterAgent();
//			if (aAgent == NULL)
//				break;
//
////			if (aType == "producer") {
////				aAgent->Disconnect(aID, mDMP11->ConsumerID());
//////				KMidiUtils::Postlude(aID);
////			}
//			if (aType == "consumer") {
//				aAgent->Disconnect(mSMFProducer->ID(), aID);
//				KMidiUtils::Postlude(aID);
//			}
//		}
//		break;

	  case K_SMF_FINISHED:
		{
			K_DEBUG_MARKER("KSMFWindow::MessageReceived() -- K_SMF_FINISHED");

			status_t aErr = B_OK;

			if (mStatus == K_PLAY) {
				NextFile();
				aErr = Load();
				if (aErr != B_OK)
					mStatus = K_STOP;
				if (mStatus == K_PLAY) {
					Start();
					UpdateFilesPopUpMenu();
				}
			}

			UpdateControlMenu();
		}
		break;

//	  case K_MIDI_MUTE:
//		{
//			status_t aErr = B_OK;
//			int8 aChannel = 0;
//			int8 aValue = 0;
//
//			aErr = msg->FindInt8("gopherwood:channel", &aChannel);
//			if (aErr != B_OK)
//				break;
//
//			aErr = msg->FindInt8("gopherwood:value", &aValue);
//			if (aErr != B_OK)
//				break;
//
//			if (aValue == 0)
//				mSMFProducer->MuteChannel(aChannel, true);
//			else
//				mSMFProducer->MuteChannel(aChannel, false);
//		}
//		break;

	  default:
		BWindow::MessageReceived(msg);
		break;
	}
}


bool
KSMFWindow::QuitRequested()
{
	K_DEBUG_MARKER("KSMFWindow::QuitRequested()");

	KMidiRosterAgent* aAgent = KMidiRosterAgent::MidiRosterAgent();
	if (aAgent != NULL)
		StopWatchingAll(aAgent);

	if (mSMFProducer != NULL) {
		if (mSMFProducer->IsRunning())
			Stop();

		BList* aConnections = NULL;
		aConnections = mSMFProducer->Connections();
		for (int i = aConnections->CountItems() - 1; 0 <= i; i--) {
			mSMFProducer->Disconnect((BMidiConsumer*)aConnections->ItemAt(i));
		}
//		mSMFProducer->Release();
//		mSMFProducer->Unregister();
	}

	be_app->PostMessage(B_QUIT_REQUESTED);

	return true;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

status_t
KSMFWindow::Load(const entry_ref& ref)
{
	K_DEBUG_MARKER("KSMFWindow::Open()");

	status_t aErr;
	BEntry aEntry;

	aErr = aEntry.SetTo(&ref, true);
	if (aErr != B_OK)
		return aErr;

	if (!aEntry.IsFile())
		return B_ERROR;

	Stop();

//	SetTitle("Now loading...");
	aErr = mSMFProducer->Load(BPath(&aEntry).Path());
	if (aErr == B_OK) {
//		mSMFProducer->Register();
//		SetTitle(BPath(&aEntry).Leaf());
		BMessage aMessage;
		mSMFProducer->GetInfo(&aMessage);
		UpdateInfo(&aMessage);

		BString aLengthString;
		aErr = aMessage.FindString("length_string", &aLengthString);
		if (aErr == B_OK) {
			BNode(&aEntry).RemoveAttr("Audio:Length");
			BNode(&aEntry).WriteAttr("Audio:Length", B_STRING_TYPE, 0, aLengthString.String(), aLengthString.Length() + 1);
		}

//		if (mStatus == K_PLAY)
//			Start();

	} else {
		(new BAlert(B_EMPTY_STRING, gApp->FindString(KR_ALERT_FAILED_TO_LOAD), "OK", NULL, NULL, B_WIDTH_AS_USUAL, B_INFO_ALERT))->Go();
	}

	return aErr;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

status_t
KSMFWindow::Load()
{
	if (mFiles.empty()) {
		mFileIndex = -1;
		return B_ERROR;
	}

	if (mFileIndex == -1)
		mFileIndex = 0;

	status_t aErr = B_OK;
	int32 aIndex = mFileIndex;

	while (true) {
		if (mFiles.empty()) {
			mFileIndex = -1;
			aErr = B_ERROR;
			break;
		}

		aErr = Load(mFiles[aIndex]);
		if (aErr == B_OK) {
			mFileIndex = aIndex;
			break;
		}

		{
			midi_files::iterator iter = mFiles.begin();
			for (int32 i = 0; i < aIndex; i++)
				iter += 1;
			mFiles.erase(iter);
		}

		if (mFiles.size() <= aIndex)
			aIndex = 0;

		if (aIndex == mFileIndex) {
			aErr = B_ERROR;
			break;
		}
	}

	return aErr;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KSMFWindow::Start()
{
	K_DEBUG_MARKER("KSMFWindow::Start()");

	if (mSMFProducer->IsRunning())
		return;

	if (!mSMFProducer->Connections()->IsEmpty() && mSMFProducer->IsLoaded())
		mSMFProducer->Start(BMessenger(this));
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KSMFWindow::Stop()
{
	K_DEBUG_MARKER("KSMFWindow::Stop()");

	if (!mSMFProducer->IsRunning())
		return;

	mSMFProducer->Stop();
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

BMenuBar*
KSMFWindow::CreateMenuBar()
{
	K_DEBUG_MARKER("KSMFWindow::CreateMenuBar()");

	BMenuBar* aMenuBar = NULL;
	BMenu* aMenu = NULL;
	BMenuItem* aMenuItem = NULL;

	aMenuBar = new BMenuBar(BRect(0, 0, 0, 0), B_EMPTY_STRING);


	//	File menu
	aMenu = new BMenu(gApp->FindString(KR_FILE_MENU));

	aMenuItem = new BMenuItem(BRecentFilesList::NewFileListMenu("Open"B_UTF8_ELLIPSIS, NULL, NULL, be_app, 32, false, kMidiType, kSignature), new BMessage(K_OPEN_REQUESTED));
	aMenuItem->SetShortcut('O', B_COMMAND_KEY);
	aMenuItem->SetTarget(be_app);
	aMenu->AddItem(aMenuItem);

	aMenu->AddSeparatorItem();

	aMenuItem = new BMenuItem(gApp->FindString(KR_QUIT_ITEM), new BMessage(B_QUIT_REQUESTED), 'Q');
	aMenuItem->SetTarget(be_app);
	aMenu->AddItem(aMenuItem);

	aMenuBar->AddItem(aMenu);


	//	Control menu
	aMenu = new BMenu(gApp->FindString(KR_CONTROL_MENU));

	aMenuItem = new BMenuItem(gApp->FindString(KR_START_ITEM), new BMessage(K_MIDI_START));
	aMenu->AddItem(aMenuItem);

	aMenuItem = new BMenuItem(gApp->FindString(KR_STOP_ITEM), new BMessage(K_MIDI_STOP));
	aMenu->AddItem(aMenuItem);

	aMenuBar->AddItem(aMenu);


	//	Ports menu
	aMenu = new BMenu(gApp->FindString(KR_PORTS_MENU));

	aMenuItem = new BMenuItem(gApp->FindString(KR_PUBLISH_ITEM), new BMessage(K_MIDI_REGISTER));
	aMenu->AddItem(aMenuItem);

	aMenu->AddSeparatorItem();

	aMenuBar->AddItem(aMenu);


	return aMenuBar;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KSMFWindow::UpdateInfo(BMessage* msg)
{
	K_DEBUG_MARKER("KSMFWindow::UpdateInfo()");

	status_t aErr = B_OK;

	BStringView* aView = NULL;

	const char* aFormat;
	int32 aNumTracks;
	int32 aLoadedTracks;
	BString aLengthString;

	aErr = msg->FindString("format", &aFormat);
	if (aErr == B_OK) {
		aView = dynamic_cast<BStringView*>(FindView("Format"));
		if (aView != NULL) {
			aView->SetText((BString("Format: ") << aFormat).String());
			aView->ResizeToPreferred();
		}
	}


	bool aIsGM = false;
	bool aIsGM2 = false;
	bool aIsGS = false;
	bool aIs88 = false;
	bool aIsXG = false;

	aErr = msg->FindBool("GM", &aIsGM);
	aErr = msg->FindBool("GM2", &aIsGM2);
	aErr = msg->FindBool("GS", &aIsGS);
	aErr = msg->FindBool("88", &aIs88);
	aErr = msg->FindBool("XG", &aIsXG);

	BString aScore;
	if (aIsGM)
		aScore << " GM";
	if (aIsGM2)
		aScore << " GM2";
	if (aIsGS)
		aScore << " GS";
	if (aIs88)
		aScore << " 88";
	if (aIsXG)
		aScore << " XG";
	if (aScore.Length() == 0)
		aScore = " unknown";

	if (aErr == B_OK) {
		aView = dynamic_cast<BStringView*>(FindView("Score"));
		if (aView != NULL) {
			aView->SetText((BString("Score :") << aScore).String());
			aView->ResizeToPreferred();
		}
	}

	PRINT(("*****************************\n"));
	if (aIsGM)
		PRINT(("GM\n"));
	if (aIsGM2)
		PRINT(("GM2\n"));
	if (aIsGS)
		PRINT(("GS\n"));
	if (aIsXG)
		PRINT(("XG\n"));
	PRINT(("*****************************\n"));


	aErr = msg->FindInt32("tracks", &aNumTracks);
	aErr = msg->FindInt32("loaded tracks", &aLoadedTracks);
	if (aErr == B_OK) {
		aView = dynamic_cast<BStringView*>(FindView("Tracks"));
		if (aView != NULL) {
			if (aNumTracks == aLoadedTracks)
				aView->SetText((BString("Tracks: ") << aNumTracks).String());
			else
				aView->SetText((BString("Tracks: ") << aNumTracks << " (" << aLoadedTracks << ")").String());
			aView->ResizeToPreferred();
		}
	}

	aErr = msg->FindString("length_string", &aLengthString);
	if (aErr == B_OK) {
		aView = dynamic_cast<BStringView*>(FindView("Length"));
		if (aView != NULL) {
			aLengthString.Prepend("Length: ");
			aView->SetText(aLengthString.String());
			aView->ResizeToPreferred();
		}
	}
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

//void
//KSMFWindow::HandleMidiEvent(BMessage* msg)
//{
//	status_t aErr = B_OK;
//	int32 aMidiOp = 0;
//
//	aErr = msg->FindInt32("be:op", &aMidiOp);
//	if (aErr != B_OK) {
//		cerr << "ERROR: unknown B_MIDI_EVENT message. (" << strerror(aErr) << ")" << endl;
//		PRINT_OBJECT((*msg));
//		return;
//	}
//
//	switch (aMidiOp) {
//	  case B_MIDI_NO_OP:
//		{
//			K_DEBUG_MARKER("KSMFWindow::HandleMidiEvent() -- B_MIDI_NO_OP");
//			PRINT_OBJECT((*msg));
//		}
//		break;
//
//	  case B_MIDI_REGISTERED:
//		{
//			K_DEBUG_MARKER("KSMFWindow::HandleMidiEvent() -- B_MIDI_REGISTERED");
//			PRINT_OBJECT((*msg));
//		}
//		break;
//
//	  case B_MIDI_UNREGISTERED:
//		{
//			K_DEBUG_MARKER("KSMFWindow::HandleMidiEvent() -- B_MIDI_UNREGISTERED");
//			PRINT_OBJECT((*msg));
//
//			if (mSMFProducer->Connections()->IsEmpty()) {
//				mStatus = K_STOP;
//				Stop();
//				UpdateControlMenu();
//			}
//		}
//		break;
//
//	  case B_MIDI_CONNECTED:
//		{
//			K_DEBUG_MARKER("KSMFWindow::HandleMidiEvent() -- B_MIDI_CONNECTED");
//			PRINT_OBJECT((*msg));
//
//#if DO_PRELUDE
//			int32 aProducerID = 0;
//			int32 aConsumerID = 0;
//			aErr = msg->FindInt32("be:producer", &aProducerID);
//			if (aErr != B_OK)
//				break;
//			aErr = msg->FindInt32("be:consumer", &aConsumerID);
//			if (aErr != B_OK)
//				break;
//
//			if (aProducerID == mSMFProducer->ID()) {
//				BMidiConsumer* aConsumer = BMidiRoster::FindConsumer(aConsumerID);
//				if (aConsumer != NULL) {
//					KMidiUtils::Prelude(aConsumer);
//					aConsumer->Release();
//				}
//			}
//#endif
//
//			UpdateControlMenu();
//		}
//		break;
//
//	  case B_MIDI_DISCONNECTED:
//		{
//			K_DEBUG_MARKER("KSMFWindow::HandleMidiEvent() -- B_MIDI_DISCONNECTED");
//			PRINT_OBJECT((*msg));
//
//			int32 aProducerID = 0;
//			int32 aConsumerID = 0;
//			aErr = msg->FindInt32("be:producer", &aProducerID);
//			if (aErr != B_OK)
//				break;
//			aErr = msg->FindInt32("be:consumer", &aConsumerID);
//			if (aErr != B_OK)
//				break;
//
//			if (aProducerID == mSMFProducer->ID()) {
//				BMidiConsumer* aConsumer = BMidiRoster::FindConsumer(aConsumerID);
//				if (aConsumer != NULL) {
//					KMidiUtils::Postlude(aConsumer);
//					aConsumer->Release();
//				}
//			}
//
//			if (mSMFProducer->Connections()->IsEmpty()) {
//				mStatus = K_STOP;
//				Stop();
//				UpdateControlMenu();
//			}
//		}
//		break;
//
//	  case B_MIDI_CHANGED_NAME:
//		{
//			K_DEBUG_MARKER("KSMFWindow::HandleMidiEvent() -- B_MIDI_CHANGED_NAME");
//			PRINT_OBJECT((*msg));
//
////			UpdatePortsMenu();
//		}
//		break;
//
//	  case B_MIDI_CHANGED_LATENCY:
//		{
//			K_DEBUG_MARKER("KSMFWindow::HandleMidiEvent() -- B_MIDI_CHANGED_LATENCY");
//			PRINT_OBJECT((*msg));
//		}
//		break;
//
//	  case B_MIDI_CHANGED_PROPERTIES:
//		{
//			K_DEBUG_MARKER("KSMFWindow::HandleMidiEvent() -- B_MIDI_CHANGED_PROPERTIES");
//			PRINT_OBJECT((*msg));
//
////			UpdatePortsMenu();
//		}
//		break;
//
//	  default:
//		{
//			K_DEBUG_MARKER("KSMFWindow::HandleMidiEvent() -- default");
//
//			PRINT_OBJECT((*msg));
//		}
//		break;
//	}
//}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

BPopUpMenu*
KSMFWindow::CreateFilesMenu()
{
	BPopUpMenu* aMenu = NULL;

	aMenu = new BPopUpMenu("Files");

	midi_files::iterator iter;
	for (iter = mFiles.begin(); iter != mFiles.end(); iter++) {
		BMenuItem* aMenuItem = NULL;
		aMenuItem = new BMenuItem((*iter).name, NULL);
		aMenu->AddItem(aMenuItem);
	}

	return aMenu;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KSMFWindow::UpdateMenus()
{
	K_DEBUG_MARKER("KSMFWindow::UpdateMenus()");

//	BAutolock aLocker(mMenuLock);

	UpdateControlMenu();
	UpdatePortsMenu();
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KSMFWindow::UpdateControlMenu()
{
	K_DEBUG_MARKER("KSMFWindow::UpdateControlMenu()");

//	BAutolock aLocker(mMenuLock);

	BMenu* aControlMenu = NULL;
	BMenuItem* aStartItem = NULL;
	BMenuItem* aStopItem = NULL;

	aControlMenu = KeyMenuBar()->SubmenuAt(KR_CONTROL_MENU / 1000 - 1);
	aStartItem = aControlMenu->ItemAt(KR_START_ITEM % 100 - 1);
	aStopItem = aControlMenu->ItemAt(KR_STOP_ITEM % 100 - 1);

	if (mSMFProducer->IsLoaded() && !mSMFProducer->Connections()->IsEmpty()) {
		aControlMenu->SetEnabled(true);
		if (mSMFProducer->IsRunning()) {
			aStartItem->SetEnabled(false);
			aStopItem->SetEnabled(true);
		} else {
			aStartItem->SetEnabled(true);
			aStopItem->SetEnabled(false);
		}
	} else {
		aControlMenu->SetEnabled(false);
	}

//	if (mSMFProducer->IsRunning()) {
//		aStartItem->SetEnabled(false);
//		aStopItem->SetEnabled(true);
//	} else {
//		if (mSMFProducer->Connections()->IsEmpty()) {
//			aStartItem->SetEnabled(false);
//		} else {
//			aStartItem->SetEnabled(true);
//		}
//		aStopItem->SetEnabled(false);
//	}
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KSMFWindow::UpdatePortsMenu()
{
	K_DEBUG_MARKER("KSMFWindow::UpdatePortsMenu()");

	BMenuItem* aItem = NULL;
	BMenu* aPortsMenu = NULL;

	aItem = KeyMenuBar()->FindItem(gApp->FindString(KR_PORTS_MENU));
	if (aItem == NULL)
		return;

	aPortsMenu = aItem->Submenu();
	if (aPortsMenu == NULL)
		return;

	aPortsMenu->RemoveItems(2, aPortsMenu->CountItems() - 2, true);

	KMidiRosterAgent* aAgent = KMidiRosterAgent::MidiRosterAgent();
	if (aAgent != NULL)
		aAgent->CreateConsumerMenu(*aPortsMenu, mSMFProducer->ID());
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KSMFWindow::UpdateFilesPopUpMenu()
{
	K_DEBUG_MARKER("KSMFWindow::UpdateFilesPopUpMenu()");

	BMenu* aFilesPopUpMenu = NULL;
	BMenuField* aFilesMenuField = NULL;

	aFilesMenuField = dynamic_cast<BMenuField*>(FindView("Files"));
	if (aFilesMenuField == NULL)
		return;


	aFilesPopUpMenu = aFilesMenuField->Menu();
	if (aFilesPopUpMenu == NULL)
		return;

	aFilesMenuField->SetEnabled(false);
	aFilesPopUpMenu->SetEnabled(false);
	UpdateIfNeeded();

	aFilesPopUpMenu->RemoveItems(0, aFilesPopUpMenu->CountItems(), true);

	if (mFiles.empty()) {
		aFilesMenuField->SetLabel("Files");
		aFilesMenuField->SetEnabled(false);
		aFilesPopUpMenu->SetEnabled(false);
		UpdateIfNeeded();
	} else {
		BMenuItem* aMenuItem = NULL;
		int32 savedIndex = mFileIndex;

		if (mFileIndex == -1)
			mFileIndex = 0;

		midi_files::iterator iter;
		for (iter = mFiles.begin(); iter != mFiles.end(); iter++) {
			aMenuItem = new BMenuItem((*iter).name, new BMessage(K_SMF_LOAD));
			aFilesPopUpMenu->AddItem(aMenuItem);
		}

		aMenuItem = aFilesPopUpMenu->ItemAt(mFileIndex);
		if (aMenuItem == NULL) {
			mFileIndex = 0;
			aMenuItem = aFilesPopUpMenu->ItemAt(0);
		}
		aMenuItem->SetMarked(true);

		if (!mSMFProducer->IsLoaded() || (savedIndex != mFileIndex)) {
			status_t aErr = B_OK;

			aErr = Load();
			if (aErr != B_OK)
				mStatus = K_STOP;
			if (mStatus == K_PLAY)
				Start();
		}

		if (!mFiles.empty()) {
			aFilesPopUpMenu->SetEnabled(true);
			aFilesMenuField->SetEnabled(true);
			UpdateIfNeeded();
		}
	}

}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KSMFWindow::NextFile()
{
	if (mFiles.empty()) {
		mFileIndex = -1;
		return;
	}

	mFileIndex++;

	if ((mFileIndex < 0) || (mFiles.size() <= mFileIndex))
		mFileIndex = 0;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KSMFWindow::RefsReceived(BMessage* msg)
{
	status_t aErr = B_OK;
	entry_ref aRef;
	type_code dummyType = B_REF_TYPE;
	int32 aCount = 0;

	aErr = msg->GetInfo("refs", &dummyType, &aCount);
	if (aErr != B_OK) {
		cerr << "ERROR (BMessage::GetInfo): " << strerror(aErr) << endl;
		return;
	}

	midi_files tmpList;

	for (int32 i = 0; i < aCount; i++) {
		aErr = msg->FindRef("refs", i, &aRef);
		if (aErr != B_OK) {
			cerr << "ERROR (BMessage::FindRef): " << strerror(aErr) << endl;
			continue;
		}

		BEntry aEntry(&aRef, true);
		if (!aEntry.Exists())
			continue;
		aEntry.GetRef(&aRef);

		if (BNode(&aRef).IsDirectory()) {
			BDirectory aDirectory(&aRef);
			BMessage aMessage(B_REFS_RECEIVED);
			aDirectory.Rewind();
			while (true) {
				BEntry tmpEntry;
				entry_ref tmpRef;

				aErr = aDirectory.GetNextEntry(&tmpEntry, true);
				if (aErr == B_ENTRY_NOT_FOUND)
					break;
				if (aErr != B_OK)
					continue;

				tmpEntry.GetRef(&tmpRef);
				aMessage.AddRef("refs", &tmpRef);
			}
			PostMessage(&aMessage);
			continue;
		}

		BMimeType aMimeType;

		aErr = BMimeType::GuessMimeType(&aRef, &aMimeType);
		if (aErr != B_OK)
			continue;

		if (!(aMimeType == kMidiType))
			continue;

		if (find(mFiles.begin(), mFiles.end(), aRef) == mFiles.end())
			tmpList.push_back(aRef);
	}

	if (!tmpList.empty()) {
		sort(tmpList.begin(), tmpList.end());
		mFiles.insert(mFiles.end(), tmpList.begin(), tmpList.end());
	}

	UpdateFilesPopUpMenu();
	UpdateControlMenu();
}

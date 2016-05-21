//======================================================================
//	KSMFProducer.cc
//----------------------------------------------------------------------
//	Copyright © 1999 KISAKA Toshihiro.
//	All rights reserved.
//		noah@gopherwood.com
//		kisaka@pochi.iijnet.or.jp
//======================================================================

#include "KSMFProducer.h"

//	System Headers
//#include <iostream>
#include <cstdio>
#include <scheduler.h>
#include <Autolock.h>
#include <AppFileInfo.h>
#include <Bitmap.h>
//#include <Debug.h>
#include <List.h>
#include <MidiConsumer.h>

//	Project Headers
//#include "SMFProducer.h"
#include "KSMFParser.h"
#include "KMidiSimpleStorage.h"

//	libKT Headers
#include "KDebug.h"
#include "KMidiUtils.h"

extern const char kGMSystemON[];
extern const char kGM2SystemON[];
extern const char kGSReset[];
extern const char k88SingleMode[];
extern const char k88DoubleMode[];
extern const char kXGSystemON[];

extern const size_t kGMSystemONLength;
extern const size_t kGM2SystemONLength;
extern const size_t kGSResetLength;
extern const size_t k88SingleModeLength;
extern const size_t k88DoubleModeLength;
extern const size_t kXGSystemONLength;


//======================================================================
#pragma mark === class KSMFProducer ===
//======================================================================

//----------------------------------------------------------------------

//----------------------------------------------------------------------

KSMFProducer::KSMFProducer(const char* name)
  : BMidiLocalProducer(name),
	mLock(true),
	mParser(new KSMFParser()),
	mStorage(new KMidiSimpleStorage()),
	mPlayerThread(-1),
	mIsRunning(false),
	mIsLoaded(false),
	mDownstreamLatency(0LL)
{
	K_DEBUG_MARKER("KSMFProducer::KSMFProducer()");

PRINT(("***** mSMFProducer->ID() = %ld\n", ID()));

#if 1
	KMidiUtils::SetupDefaultProperties(*this);
#else
	status_t aErr = B_OK;
	BMessage aProperties;
	BAppFileInfo aInfo;
	BBitmap* aLargeIcon = NULL;
	BBitmap* aMiniIcon = NULL;

	aLargeIcon = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	aMiniIcon = new BBitmap(BRect(0, 0, 15, 15), B_CMAP8);

	aErr = aInfo.GetIcon(aLargeIcon, B_LARGE_ICON);
	if (aErr != B_OK)
		BMimeType(B_APP_MIME_TYPE).GetIcon(aLargeIcon, B_LARGE_ICON);

	aErr = aInfo.GetIcon(aMiniIcon, B_MINI_ICON);
	if (aErr != B_OK)
		BMimeType(B_APP_MIME_TYPE).GetIcon(aMiniIcon, B_MINI_ICON);

	GetProperties(&aProperties);

	aErr = aProperties.ReplaceData("be:large_icon", 'ICON', aLargeIcon->Bits(), aLargeIcon->BitsLength());
	if (aErr != B_OK)
		aErr = aProperties.AddData("be:large_icon", 'ICON', aLargeIcon->Bits(), aLargeIcon->BitsLength());

	aErr = aProperties.ReplaceData("be:mini_icon", 'MICN', aMiniIcon->Bits(), aMiniIcon->BitsLength());
	if (aErr != B_OK)
		aErr = aProperties.AddData("be:mini_icon", 'MICN', aMiniIcon->Bits(), aMiniIcon->BitsLength());

//	aErr = aProperties.ReplaceString("gopherwood:group_name", mGroupName);
//	if (aErr != B_OK)
//		aProperties.AddString("gopherwood:group_name", mGroupName);

	SetProperties(&aProperties);

	delete aLargeIcon;
	delete aMiniIcon;
#endif
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

KSMFProducer::~KSMFProducer()
{
	K_DEBUG_MARKER("KSMFProducer::~KSMFProducer()");

	if (IsRunning())
		Stop();

	delete mParser;
//	delete mStorage;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KSMFProducer::Connected(BMidiConsumer* dest)
{
	K_DEBUG_MARKER("KSMFProducer::Connected()");

//	if (dest != NULL) {
//		bigtime_t aLatency = dest->Latency();
//		if (mDownstreamLatency < aLatency)
//			mDownstreamLatency = aLatency;
//
//		PRINT(("dest->Latency() = %Ld\n", aLatency));
//		PRINT(("mDownstreamLatency = %Ld\n", mDownstreamLatency));
//	}

	BMidiLocalProducer::Connected(dest);
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KSMFProducer::Disconnected(BMidiConsumer* dest)
{
	K_DEBUG_MARKER("KSMFProducer::Disconnected()");

//	BList* aConsumers = Connections();
//	if (aConsumers != NULL) {
//		bigtime_t aLatency = 0LL;
//		for (int i = 0; ; i++) {
//			BMidiConsumer* aConsumer = reinterpret_cast<BMidiConsumer*>(aConsumers->ItemAt(i));
//			if (aConsumer == NULL)
//				break;
//			if (aConsumer == dest)
//				continue;
//			bigtime_t tmpLatency = aConsumer->Latency();
//			if (aLatency < tmpLatency)
//				aLatency = tmpLatency;
//		}
//		mDownstreamLatency = aLatency;
//	}

	PRINT(("mDownstreamLatency = %Ld\n", mDownstreamLatency));

	BMidiLocalProducer::Disconnected(dest);
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

status_t
KSMFProducer::Load(const char* file)
{
	K_DEBUG_MARKER("KSMFProducer::Load()");

	BAutolock lock(&mLock);

	if (!IsLocked())
		debugger("ERROR - you must Lock object before calling Start()");

	if (mIsRunning)
		Stop();

//	delete mStorage;
//	mStorage = NULL;
//
//	mStorage = new KMidiSimpleStorage();
	auto_ptr<KMidiStorage> aStorage(new KMidiSimpleStorage());

	status_t aErr = B_OK;
	aErr = mParser->Load(file, aStorage.get());
	if (aErr != B_OK)
		return aErr;

#if DEBUG
	uint32 tmp = aStorage->CountEvents();
	PRINT(("events = %ul\n", tmp));
#endif

	aStorage->SortEvents();
//	mStorage->DumpEvents();

	mStorage = aStorage;
	mIsLoaded = true;

	return B_OK;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

status_t
KSMFProducer::Start()
{
	K_DEBUG_MARKER("KSMFProducer::Start()");

	return Start(BMessenger());
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

status_t
KSMFProducer::Start(BMessenger messenger)
{
	K_DEBUG_MARKER("KSMFProducer::Start(BMessenger)");

	BAutolock lock(&mLock);

	if (!IsLocked())
		debugger("ERROR - you must Lock object before calling Start()");

	if (IsRunning() || !IsLoaded())
		return B_ERROR;

	if (Connections()->IsEmpty()) {
		fprintf(stderr, "ERROR: NOT CONNECTED\n");
		return B_ERROR;
	}

	status_t aErr = B_OK;
	thread_id aThread = -1;

	aThread = spawn_thread((thread_func)KSMFProducer::player, "player", B_URGENT_DISPLAY_PRIORITY, this);
	if (aThread < B_OK) {
		aErr = aThread;
	} else {
		mIsRunning = true;
		mPlayerThread = aThread;
		resume_thread(aThread);
	}

	mObserver = messenger;

	return aErr;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KSMFProducer::Stop()
{
	K_DEBUG_MARKER("KSMFProducer::Stop()");

	BAutolock lock(&mLock);

	if (!IsLocked())
		debugger("ERROR - you must Lock object before calling Start()");

	status_t aErr = B_OK;

	mIsRunning = false;
	wait_for_thread(mPlayerThread, &aErr);
	mPlayerThread = -1;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

int32
KSMFProducer::player(KSMFProducer* arg)
{
	K_DEBUG_MARKER("KSMFProducer::player()");

	status_t aErr = B_OK;

	if (arg == NULL) {
		aErr = B_ERROR;
	} else {
		KSMFProducer* aSMF = arg;
		aErr = aSMF->Player();
	}

	return aErr;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

int32
KSMFProducer::Player()
{
	K_DEBUG_MARKER("KSMFProducer::Player()");

	mStorage->SeekByTick(0);

	Prelude();

//	BMessage aMessage;
//	GetInfo(&aMessage);
//	bigtime_t aLength = 0;
//	aMessage.FindInt64("length", &aLength);
//
	bigtime_t aLatency2 = estimate_max_scheduling_latency() * 2;
	if (1000 < aLatency2)
		aLatency2 = 1000;

	bigtime_t aStartTime = system_time();
	bigtime_t aSnoozeTime = 0;
	bigtime_t aEventTime = 0;

	while (mIsRunning) {
		const KMidiEvent* aEvent = mStorage->NextMidiEvent();

		if (aEvent == NULL) {
			mIsRunning = false;
			goto all_done;
		}

		aEventTime = aStartTime + mStorage->Time(aEvent->mTick);
		while (true) {
			aSnoozeTime = aEventTime - system_time();
			if (aSnoozeTime < aLatency2 + mDownstreamLatency)
				break;
			snooze(aLatency2 < aSnoozeTime ? aLatency2 : aSnoozeTime);
			if (!mIsRunning)
				goto gesyo;
		}

		switch (aEvent->mType) {
		  case B_NOTE_OFF:
			SprayNoteOff(aEvent->mChannel, aEvent->mData[0], aEvent->mData[1], aEventTime);
			break;
		  case B_NOTE_ON:
			SprayNoteOn(aEvent->mChannel, aEvent->mData[0], aEvent->mData[1], aEventTime);
			break;
		  case B_KEY_PRESSURE:
			SprayKeyPressure(aEvent->mChannel, aEvent->mData[0], aEvent->mData[1], aEventTime);
			break;
		  case B_CONTROL_CHANGE:
			SprayControlChange(aEvent->mChannel, aEvent->mData[0], aEvent->mData[1], aEventTime);
			break;
		  case B_PROGRAM_CHANGE:
			SprayProgramChange(aEvent->mChannel, aEvent->mData[0], aEventTime);
			break;
		  case B_CHANNEL_PRESSURE:
			SprayChannelPressure(aEvent->mChannel, aEvent->mData[0], aEventTime);
			break;
		  case B_PITCH_BEND:
			SprayPitchBend(aEvent->mChannel, aEvent->mData[0], aEvent->mData[1], aEventTime);
			break;
		  case 0xF0:
//			fprintf(stderr, "System Exclusive!\n");
#if 1
			SpraySystemExclusive((void*)aEvent->mExtData, aEvent->mLength, aEventTime);
//			SpraySystemExclusive((void*)aEvent->mData, aEvent->mLength - 1, aEventTime);
#else
			{
				char a;
				a = 0xF0;
				SprayData(&a, 1);
				SprayData((void*)aEvent->mData, aEvent->mLength, true, aEventTime);
			}
#endif
			break;
		  case 0xF7:
			SprayData((void*)aEvent->mExtData, aEvent->mLength, true, aEventTime);
			break;
		  default:
			fprintf(stderr, "***** unknown event *****\n");
			break;
		}
	}

  gesyo:
	Postlude();

	return B_OK;

  all_done:
	snooze(500000);
	Postlude();
	Finished();

	return B_OK;
}


void
KSMFProducer::Prelude()
{
	K_DEBUG_MARKER("KSMFProducer::Prelude()");

	SpraySystemExclusive((void*)kGMSystemON, kGMSystemONLength, system_time());
   	snooze(100000);
	SpraySystemExclusive((void*)kGM2SystemON, kGM2SystemONLength, system_time());
   	snooze(100000);
	SpraySystemExclusive((void*)kGSReset, kGSResetLength, system_time());
   	snooze(100000);
//	SpraySystemExclusive((void*)kXGSystemON, sizeof kXGSystemONLength, system_time());
//   	snooze(100000);
}


void
KSMFProducer::Postlude()
{
	K_DEBUG_MARKER("KSMFProducer::Postlude()");

	for (int i = 0; i < 16; i++) {
		//	all note off
		SprayControlChange(i, B_ALL_NOTES_OFF, 0 , system_time());
		//	all sound off
		SprayControlChange(i, 0x78, 0, system_time());
		//	all control reset
		SprayControlChange(i, B_RESET_ALL_CONTROLLERS, 0, system_time());
	}
}


void
KSMFProducer::Finished()
{
	K_DEBUG_MARKER("KSMFProducer::Finished()");

	mObserver.SendMessage(K_SMF_FINISHED);
}


//void
//KSMFProducer::MuteChannel(uint8 channel, bool mute)
//{
//	if (channel < 16)
//		mIsMuted[channel] = mute;
//}


void
KSMFProducer::GetInfo(BMessage* msg) const
{
	K_DEBUG_MARKER("KSMFProducer::GetInfo()");

	mStorage->GetInfo(msg);
}


bool
KSMFProducer::Lock()
{
	return mLock.Lock();
}


void
KSMFProducer::Unlock()
{
	mLock.Unlock();
}


bool
KSMFProducer::IsLocked() const
{
	return mLock.IsLocked();
}


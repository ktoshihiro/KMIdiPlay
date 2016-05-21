//======================================================================
//	File: KMidiSimpleStorage.cc
//----------------------------------------------------------------------
//	Copyright © 1999 KISAKA Toshihiro.
//	All rights reserved.
//		noah@gopherwood.com
//		kisaka@pochi.iijnet.or.jp
//======================================================================

#include "KMidiSimpleStorage.h"

//	System Headers
#include <iostream>
#include <algorithm>
#include <Debug.h>
#include <Message.h>
#include <Midi2Defs.h>

#include "SMFProducer.h"


//======================================================================
#pragma mark === class KMidiEvent ===
//======================================================================

//----------------------------------------------------------------------

//----------------------------------------------------------------------

KMidiEvent::KMidiEvent()
  : mDelta(0),
	mTick(0),
	mType(0),
	mChannel(0),
	mLength(0),
	mExtData(NULL)
{
	mData[0] = 0;
	mData[1] = 0;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

KMidiEvent::KMidiEvent(uint32 delta, uint32 tick, uint8 type, uint8 channel, uint8 data1, uint8 data2)
  : mDelta(delta),
	mTick(tick),
	mType(type),
	mChannel(channel),
	mLength(0),
	mExtData(NULL)
{
	mData[0] = 0;
	mData[1] = 0;

	switch (mType) {
	  case B_NOTE_OFF:
	  case B_NOTE_ON:
	  case B_KEY_PRESSURE:
	  case B_CONTROL_CHANGE:
	  case B_PROGRAM_CHANGE:
	  case B_CHANNEL_PRESSURE:
	  case B_PITCH_BEND:
		{
			mData[0] = data1;
			mData[1] = data2;
		}
		break;

	  default:
		{
			throw;
		}
		break;
	}
}


KMidiEvent::KMidiEvent(uint32 delta, uint32 tick, uint8 type, uint8 channel, uint32 length, uint8* data)
  : mDelta(delta),
	mTick(tick),
	mType(type),
	mChannel(channel),
	mLength(length),
	mExtData(NULL)
{
	mData[0] = 0;
	mData[1] = 0;

	switch (mType) {
	  case B_NOTE_OFF:
	  case B_NOTE_ON:
	  case B_KEY_PRESSURE:
	  case B_CONTROL_CHANGE:
	  case B_PROGRAM_CHANGE:
	  case B_CHANNEL_PRESSURE:
	  case B_PITCH_BEND:
		{
			throw;
		}
		break;

	  default:
		{
			mExtData = data;
		}
		break;
	}
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

KMidiEvent::KMidiEvent(const KMidiEvent& event)
  : mDelta(event.mDelta),
	mTick(event.mTick),
	mType(event.mType),
	mChannel(event.mChannel),
	mLength(event.mLength),
	mExtData(NULL)
{
	mData[0] = 0;
	mData[1] = 0;

	switch (mType) {
	  case B_NOTE_OFF:
	  case B_NOTE_ON:
	  case B_KEY_PRESSURE:
	  case B_CONTROL_CHANGE:
	  case B_PROGRAM_CHANGE:
	  case B_CHANNEL_PRESSURE:
	  case B_PITCH_BEND:
		{
			mData[0] = event.mData[0];
			mData[1] = event.mData[1];
		}
		break;

	  default:
		{
			mExtData = new uint8[mLength];
			memcpy((void*)mExtData, event.mExtData, mLength);
		}
		break;
	}
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

KMidiEvent::~KMidiEvent()
{
	delete[] mExtData;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

KMidiEvent& 
KMidiEvent::operator=(const KMidiEvent& rhs)
{
	if (&rhs != this) {
		mDelta = rhs.mDelta;
		mTick = rhs.mTick;
		mType = rhs.mType;
		mChannel = rhs.mChannel;
		mLength = rhs.mLength;

		mData[0] = 0;
		mData[1] = 0;

		delete[] mExtData;
		mExtData = NULL;

		switch (mType) {
		  case B_NOTE_OFF:
		  case B_NOTE_ON:
		  case B_KEY_PRESSURE:
		  case B_CONTROL_CHANGE:
		  case B_PROGRAM_CHANGE:
		  case B_CHANNEL_PRESSURE:
		  case B_PITCH_BEND:
			{
				mData[0] = rhs.mData[0];
				mData[1] = rhs.mData[1];
			}
			break;

		  default:
			{
				mExtData = new uint8[mLength];
				memcpy((void*)mExtData, rhs.mExtData, mLength);
			}
			break;
		}
	}

	return *this;
}


////----------------------------------------------------------------------
//
////----------------------------------------------------------------------
//
//bool
//KMidiEvent::operator<(const KMidiEvent& event) const
//{
//	return (mTick < event.mTick);
//}


//======================================================================
#pragma mark === class KMidiSimpleStorage ===
//======================================================================

//----------------------------------------------------------------------

//----------------------------------------------------------------------

KMidiSimpleStorage::KMidiSimpleStorage()
  : KMidiStorage(),
	mCurrentEvent(mEvents.begin()),
	mEOTTick(0)
{
	tempo_map aTempo = {0, 0, 500000};
	mTempoMap.push_back(aTempo);
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

KMidiSimpleStorage::~KMidiSimpleStorage()
{
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KMidiSimpleStorage::HandleMidiEvent(uint32 delta, uint32 tick, uint8 type, uint8 channel, uint8 data1, uint8 data2)
{
	switch (type) {
	  case B_NOTE_ON:
		if (data2 == 0) {
			type = B_NOTE_OFF;
			data2 = 64;
		}
		//	break;
	  case B_NOTE_OFF:
	  case B_KEY_PRESSURE:
	  case B_CONTROL_CHANGE:
	  case B_PITCH_BEND:
		{
			mEvents.push_back(KMidiEvent(delta, tick * 480 / TimeFormat(), type, channel, data1, data2));
		}
		break;
	  case B_PROGRAM_CHANGE:
	  case B_CHANNEL_PRESSURE:
		{
			mEvents.push_back(KMidiEvent(delta, tick * 480 / TimeFormat(), type, channel, data1));
		}
		break;
	  default:
		cerr << "***** unknown event *****" << endl;
		break;
	}
/*
	switch (status) {
	  case B_NOTE_OFF:
		cout << "*** " << delta << "(" << tick << ")" << ": note off; channel = " << (int)channel << ", note = " << (int)data1 << ", velocity = " << (int)data2 << " ***"<< endl;
		break;
	  case B_NOTE_ON:
		cout << "*** " << delta << "(" << tick << ")" << ": note on; channel = " << (int)channel << ", note = " << (int)data1 << ", velocity = " << (int)data2 << " ***"<< endl;
		break;
	  case B_KEY_PRESSURE:
		cout << "*** " << delta << "(" << tick << ")" << ": polyphonic key pressure; channel = " << (int)channel << ", data1 = " << (int)data1 << ", data2 = " << (int)data2 << " ***"<< endl;
		break;
	  case B_CONTROL_CHANGE:
		cout << "*** " << delta << "(" << tick << ")" << ": B_CONTROL_CHANGE; channel = " << (int)channel << ", control = " << (int)data1 << ", value = " << (int)data2 << " ***"<< endl;
		break;
	  case B_PROGRAM_CHANGE:
		cout << "*** " << delta << "(" << tick << ")" << ": program change; channel = " << (int)channel << ", program = " << (int)data1 << " ***"<< endl;
		break;
	  case B_CHANNEL_PRESSURE:
		cout << "*** " << delta << "(" << tick << ")" << ": chunnel pressure; channel = " << (int)channel << ", data1 = " << (int)data1 << " ***"<< endl;
		break;
	  case B_PITCH_BEND:
		cout << "*** " << delta << "(" << tick << ")" << ": pitch bend change; channel = " << (int)channel << ", lsb = " << (int)data1 << ", msb = " << (int)data2 << " ***"<< endl;
		break;
	  default:
		cout << "not yet" << endl;
		break;
	}
*/
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KMidiSimpleStorage::HandleSystemExclusive(uint32 delta, uint32 tick, uint8 type, uint32 length, uint8* data)
{
	mEvents.push_back(KMidiEvent(delta, tick * 480 / TimeFormat(), type, 0xFF, length, data));
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KMidiSimpleStorage::HandleTempoChange(uint32 delta, uint32 tick, uint32 quarter_note_length)
{
	tempo_map aTempo = {tick * 480 / TimeFormat(), 0, quarter_note_length};
	mTempoMap.push_back(aTempo);
//	UpdateTime();
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KMidiSimpleStorage::HandleMetaEvent(uint32 delta, uint32 tick, uint8 type, uint32 length, uint8* data)
{
	switch (type) {
	  case 0x2F:	//	end of track
		if (mEOTTick < tick)
			mEOTTick = tick;
		break;
	  default:
		break;
	}
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KMidiSimpleStorage::UpdateTime()
{
	uint32 aLastTick = 0;
	bigtime_t aLastTime = 0;
	uint32 aLastQNoteLength = 0;

	tempo_map_t::iterator iter;

	for (iter = mTempoMap.begin(); iter != mTempoMap.end(); ++iter)
	{
		(*iter).mTime = aLastTime + bigtime_t((*iter).mTick - aLastTick) * aLastQNoteLength / 480;
		aLastTick = (*iter).mTick;
		aLastTime = (*iter).mTime;
		aLastQNoteLength = (*iter).mQNoteLength;
#if DEBUG
		fprintf(stdout, "%lu (%f), %Ld, %lu\n", aLastTick, aLastTick / 480.0, aLastTime, aLastQNoteLength);
		fflush(stdout);
#endif
	}
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

bigtime_t
KMidiSimpleStorage::Time(uint32 tick) const
{
	tempo_map_t::const_iterator iter;

	for (iter = mTempoMap.begin(); iter != mTempoMap.end(); ++iter)
	{
		if ((*iter).mTick > tick) break;
	}
	iter--;
	return (*iter).mTime + bigtime_t(tick - (*iter).mTick) * (*iter).mQNoteLength / 480;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

uint32
KMidiSimpleStorage::Tick(bigtime_t time) const
{
	tempo_map_t::const_iterator iter;

	for (iter = mTempoMap.begin(); iter != mTempoMap.end(); ++iter)
	{
		if ((*iter).mTime > time) break;
	}
	iter--;
	return (*iter).mTick + uint32(time - (*iter).mTime) * 480 / (*iter).mQNoteLength;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

uint32
KMidiSimpleStorage::CountEvents() const
{
	return mEvents.size();
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KMidiSimpleStorage::DumpEvents()
{
#if DEBUG
	PRINT(("KMidiSimpleStorage::DumpEvents()\n"));

	vector<KMidiEvent>::const_iterator iter;
	for (iter = mEvents.begin(); iter != mEvents.end(); iter++) {
		switch ((*iter).mType) {
		  case B_NOTE_OFF:
			cout << "*** " << (*iter).mDelta << "(" << (*iter).mTick << ")" << ": note off; channel = " << (int)(*iter).mChannel << ", note = " << (int)(*iter).mData[0] << ", velocity = " << (int)(*iter).mData[1] << " ***"<< endl;
			break;
		  case B_NOTE_ON:
			cout << "*** " << (*iter).mDelta << "(" << (*iter).mTick << ")" << ": note on; channel = " << (int)(*iter).mChannel << ", note = " << (int)(*iter).mData[0] << ", velocity = " << (int)(*iter).mData[1] << " ***"<< endl;
			break;
		  case B_KEY_PRESSURE:
			cout << "*** " << (*iter).mDelta << "(" << (*iter).mTick << ")" << ": polyphonic key pressure; channel = " << (int)(*iter).mChannel << ", data1 = " << (int)(*iter).mData[0] << ", data2 = " << (int)(*iter).mData[1] << " ***"<< endl;
			break;
		  case B_CONTROL_CHANGE:
			cout << "*** " << (*iter).mDelta << "(" << (*iter).mTick << ")" << ": B_CONTROL_CHANGE; channel = " << (int)(*iter).mChannel << ", control = " << (int)(*iter).mData[0] << ", value = " << (int)(*iter).mData[1] << " ***"<< endl;
			break;
		  case B_PROGRAM_CHANGE:
			cout << "*** " << (*iter).mDelta << "(" << (*iter).mTick << ")" << ": program change; channel = " << (int)(*iter).mChannel << ", program = " << (int)(*iter).mData[0] << " ***"<< endl;
			break;
		  case B_CHANNEL_PRESSURE:
			cout << "*** " << (*iter).mDelta << "(" << (*iter).mTick << ")" << ": chunnel pressure; channel = " << (int)(*iter).mChannel << ", data1 = " << (int)(*iter).mData[0] << " ***"<< endl;
			break;
		  case B_PITCH_BEND:
			cout << "*** " << (*iter).mDelta << "(" << (*iter).mTick << ")" << ": pitch bend change; channel = " << (int)(*iter).mChannel << ", lsb = " << (int)(*iter).mData[0] << ", msb = " << (int)(*iter).mData[1] << " ***"<< endl;
			break;
		  case B_SYS_EX_START:
			cout << "*** " << (*iter).mDelta << "(" << (*iter).mTick << ")" << ": System Exclusive" << endl;
			break;
		  default:
			cout << "unknown" << endl;
			break;
		}
	}
#endif
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KMidiSimpleStorage::SortEvents()
{
	stable_sort(mTempoMap.begin(), mTempoMap.end());
	UpdateTime();
	stable_sort(mEvents.begin(), mEvents.end());
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KMidiSimpleStorage::SeekByTick(uint32 tick)
{
	vector<KMidiEvent>::iterator iter;

	for (iter = mEvents.begin(); iter != mEvents.end(); iter++) {
		if (tick <= (*iter).mTick) {
			mCurrentEvent = iter;
			break;
		}
	}
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

const KMidiEvent*
KMidiSimpleStorage::NextMidiEvent()
{
//	PRINT(("KMidiSimpleStorage::NextMidiEvent()\n"));

	if (mCurrentEvent == mEvents.end())
		return NULL;
	else
		return &(*mCurrentEvent++);
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KMidiSimpleStorage::GetInfo(BMessage* msg) const
{
	msg->AddString("format", Format());
	msg->AddInt32("tracks", NumTracks());
	msg->AddInt32("loaded tracks", LoadedTracks());

	uint32 aTick;
	bigtime_t aLength;
	aTick = (*mEvents.rbegin()).mTick;
	if (aTick < mEOTTick)
		aTick = mEOTTick;
	aLength = Time(aTick);
	msg->AddInt64("length", aLength);

	char aLengthString[64] = "";
	sprintf(aLengthString, "%02Ld:%02Ld", aLength / 1000000 / 60, aLength / 1000000 % 60);
	msg->AddString("length_string", aLengthString);

	bool aIsGM = false;
	bool aIsGM2 = false;
	bool aIsGS = false;
	bool aIs88 = false;
	bool aIsXG = false;
	vector<KMidiEvent>::const_iterator iter;
	for (iter = mEvents.begin(); iter != mEvents.end(); iter++) {
		if ((*iter).mType != 0xF0)
			continue;

		if ((*iter).mLength == kGMSystemONLength + 1)
			if (memcmp((*iter).mExtData, kGMSystemON, kGMSystemONLength) == 0)
				aIsGM = true;

		if ((*iter).mLength == kGM2SystemONLength + 1)
			if (memcmp((*iter).mExtData, kGM2SystemON, kGM2SystemONLength) == 0)
				aIsGM2 = true;

		if ((*iter).mLength == kGSResetLength + 1)
			if (memcmp((*iter).mExtData, kGSReset, kGSResetLength) == 0)
				aIsGS = true;

		if ((*iter).mLength == k88SingleModeLength + 1)
			if (memcmp((*iter).mExtData, k88SingleMode, k88SingleModeLength) == 0)
				aIs88 = true;

		if ((*iter).mLength == k88DoubleModeLength + 1)
			if (memcmp((*iter).mExtData, k88DoubleMode, k88DoubleModeLength) == 0)
				aIs88 = true;

		if ((*iter).mLength == kXGSystemONLength + 1)
			if (memcmp((*iter).mExtData, kXGSystemON, kXGSystemONLength) == 0)
				aIsXG = true;

		for (uint i = 0; i < (*iter).mLength; i++)
			PRINT((" %02X", (*iter).mExtData[i]));
		PRINT(("\n"));
	}

	msg->AddBool("GM", aIsGM);
	msg->AddBool("GM2", aIsGM2);
	msg->AddBool("GS", aIsGS);
	msg->AddBool("88", aIs88);
	msg->AddBool("XG", aIsXG);
}

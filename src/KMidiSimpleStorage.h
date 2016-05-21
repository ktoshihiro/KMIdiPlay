//======================================================================
//	KMidiSimpleStorage.h
//----------------------------------------------------------------------
//	Copyright © 1999 KISAKA Toshihiro.
//	All rights reserved.
//		noah@gopherwood.com
//		kisaka@pochi.iijnet.or.jp
//======================================================================

#ifndef __KSMFSimpleStorage_h__
#define __KSMFSimpleStorage_h__

#include "KMidiStorage.h"

//	System Headers
#include <vector>
#include <list>


struct tempo_map {
	uint32 mTick;
	bigtime_t mTime;
	uint32 mQNoteLength;

	bool operator<(const tempo_map& rhs) const
	{
		return (mTick < rhs.mTick);
	}
};

//======================================================================
#pragma mark === class KMidiEvent ===
//======================================================================

struct KMidiEvent
{
  public:
	KMidiEvent();
	KMidiEvent(uint32 delta, uint32 tick, uint8 type, uint8 channel, uint8 data1, uint8 data2 = 0);
	KMidiEvent(uint32 delta, uint32 tick, uint8 type, uint8 channel, uint32 length, uint8* data);
	KMidiEvent(const KMidiEvent& event);
	virtual ~KMidiEvent();

	KMidiEvent& operator=(const KMidiEvent& event);
	bool operator<(const KMidiEvent& rhs) const
	{
		return (mTick < rhs.mTick);
	}

	uint32 mDelta;
	uint32 mTick;
	uint8 mType;
	uint8 mChannel;
	uint32 mLength;

		uint8 mData[2];
		const uint8* mExtData;

  protected:

  private:
	bool operator==(const KMidiEvent&);	//	Not impremented
	bool operator<=(const KMidiEvent&);	//	Not impremented
	bool operator>(const KMidiEvent&);	//	Not impremented
	bool operator>=(const KMidiEvent&);	//	Not impremented
};


//======================================================================
#pragma mark === class KMidiSimpleStorage ===
//======================================================================

class KMidiSimpleStorage
  : public KMidiStorage
{
  public:
	KMidiSimpleStorage();
	virtual ~KMidiSimpleStorage();

	virtual void HandleMidiEvent(uint32 delta, uint32 tick, uint8 type, uint8 channel, uint8 data1 = 64, uint8 data2 = 64);
	virtual void HandleSystemExclusive(uint32 delta, uint32 tick, uint8 type, uint32 length, uint8* data);
	virtual void HandleTempoChange(uint32 delta, uint32 tick, uint32 quarter_note_length);
	virtual void HandleMetaEvent(uint32 delta, uint32 tick, uint8 type, uint32 length, uint8* data);

	virtual uint32 CountEvents() const;
	virtual void DumpEvents();
	virtual void SortEvents();

	virtual void SeekByTick(uint32 tick);
	virtual const KMidiEvent* NextMidiEvent();

	virtual bigtime_t Time(uint32 tick) const;
	virtual uint32 Tick(bigtime_t time) const;

	virtual void GetInfo(BMessage* msg) const;

  protected:

  private:
	KMidiSimpleStorage(const KMidiSimpleStorage&);	//	Not impremented
	KMidiSimpleStorage& operator=(const KMidiSimpleStorage&);	//	Not impremented

	vector<KMidiEvent> mEvents;
	vector<KMidiEvent>::iterator mCurrentEvent;

	void UpdateTime();
	typedef vector<tempo_map> tempo_map_t;
	tempo_map_t mTempoMap;

	uint32 mEOTTick;
};

#endif

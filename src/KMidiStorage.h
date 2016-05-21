//======================================================================
//	File: KMidiStorage.h
//----------------------------------------------------------------------
//	Copyright © 1999 KISAKA Toshihiro.
//	All rights reserved.
//		noah@gopherwood.com
//		kisaka@pochi.iijnet.or.jp
//======================================================================

#ifndef __KMidiStorage_h__
#define __KMidiStorage_h__

#include <SupportDefs.h>
#include <String.h>

#include "KSMFProducer.h"

class KMidiEvent;


//======================================================================
#pragma mark === class KMidiStorage ===
//======================================================================

class KMidiStorage
{
  public:
	KMidiStorage();
	virtual ~KMidiStorage();

	void SetFormat(const char* format);
	const char* Format() const
	{
		return mFormat.String();
	}

	void SetNumTracks(uint32 num);
	int32 NumTracks() const
	{
		return mNumTracks;
	}

	void SetLoadedTracks(uint32 num);
	int32 LoadedTracks() const
	{
		return mLoadedTracks;
	}

	void SetTimeFormat(int32 timeformat);
	int32 TimeFormat() const
	{
		return mTimeFormat;
	}

	virtual void HandleMidiEvent(uint32 delta, uint32 tick, uint8 type, uint8 channel, uint8 data1 = 64, uint8 data2 = 64) = 0;
	virtual void HandleSystemExclusive(uint32 delta, uint32 tick, uint8 type, uint32 length, uint8* data) = 0;
	virtual void HandleTempoChange(uint32 delta, uint32 tick, uint32 quarter_note_length) = 0;
	virtual void HandleMetaEvent(uint32 delta, uint32 tick, uint8 type, uint32 length, uint8* data) = 0;

	virtual uint32 CountEvents() const = 0;
	virtual void DumpEvents();
	virtual void SortEvents() = 0;

	virtual void SeekByTick(uint32 tick);
	virtual const KMidiEvent* NextMidiEvent() = 0;

	virtual bigtime_t Time(uint32 tick) const = 0;
	virtual uint32 Tick(bigtime_t time) const = 0;

	virtual void GetInfo(BMessage* msg) const = 0;

  protected:

  private:
//	KMidiStorage();	//	Not impremented
	KMidiStorage(const KMidiStorage&);	//	Not impremented
	KMidiStorage& operator=(const KMidiStorage&);	//	Not impremented

	BString mFormat;
	uint32 mNumTracks;
	uint32 mLoadedTracks;
	int32 mTimeFormat;
};

#endif

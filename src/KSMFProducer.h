//======================================================================
//	KSMFProducer.h
//----------------------------------------------------------------------
//	Copyright © 1999 KISAKA Toshihiro.
//	All rights reserved.
//		noah@gopherwood.com
//		kisaka@pochi.iijnet.or.jp
//======================================================================

#ifndef __KSMFProducer_h__
#define __KSMFProducer_h__

#include <MidiProducer.h>

//	System Headers
#include <memory>
#include <Messenger.h>

//	Forward Declarations
class KSMFParser;
class KMidiStorage;

enum {
	K_SMF_FINISHED = 'smff'
};


const char kGMSystemON[] = {0x7E, 0x7F, 0x09, 0x01};
const char kGM2SystemON[] = {0x7E, 0x7F, 0x09, 0x03};
const char kGSReset[] = {0x41, 0x10, 0x42, 0x12, 0x40, 0x00, 0x7F, 0x00, 0x41};
const char k88SingleMode[] = {0x41, 0x10, 0x42, 0x12, 0x00, 0x00, 0x7F, 0x00, 0x01};
const char k88DoubleMode[] = {0x41, 0x10, 0x42, 0x12, 0x00, 0x00, 0x7F, 0x01, 0x00};
const char kXGSystemON[] = {0x43, 0x10, 0x4C, 0x00, 0x00, 0x7E, 0x00};

const size_t kGMSystemONLength = sizeof kGMSystemON;
const size_t kGM2SystemONLength = sizeof kGM2SystemON;
const size_t kGSResetLength = sizeof kGSReset;
const size_t k88SingleModeLength = sizeof k88SingleMode;
const size_t k88DoubleModeLength = sizeof k88DoubleMode;
const size_t kXGSystemONLength = sizeof kXGSystemON;

//======================================================================
#pragma mark === class KSMFProducer ===
//======================================================================

class KSMFProducer
  : public BMidiLocalProducer
{
  public:
	KSMFProducer(const char* name);

	//	BMidiLocalProducer
	virtual void Connected(BMidiConsumer* dest);
	virtual void Disconnected(BMidiConsumer* dest);

	virtual status_t Load(const char* file);

	virtual status_t Start();
	virtual status_t Start(BMessenger messenger);
	virtual void Prelude();
	virtual void Postlude();
	virtual void Stop();
	virtual void Finished();

//	void MuteChannel(uint8 channel, bool mute);

	bool IsRunning() const
	{
		return mIsRunning;
	}

	bool IsLoaded() const
	{
		return mIsLoaded;
	}

	void GetInfo(BMessage* msg) const;

	bool Lock();
	void Unlock();
	bool IsLocked() const;

  protected:
	virtual ~KSMFProducer();

  private:
	KSMFProducer();	//	Not impremented
	KSMFProducer(const KSMFProducer&);	//	Not impremented
	KSMFProducer& operator=(const KSMFProducer&);	//	Not impremented

	static int32 player(KSMFProducer* arg);
	int32 Player();

	BLocker mLock;

	BMessenger mObserver;

	KSMFParser* mParser;
	auto_ptr<KMidiStorage> mStorage;

	thread_id mPlayerThread;
	bool mIsRunning;

	bool mIsLoaded;

	bigtime_t mDownstreamLatency;
};

#endif

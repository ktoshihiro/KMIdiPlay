//	File: KMidiThrough.h

#ifndef __KMidiThrough_h__
#define __KMidiThrough_h__

#include <MidiConsumer.h>

//	System Headers
#include <Locker.h>
#include <Messenger.h>

//	Forward Declarations
class BMidiLocalProducer;

enum {
	K_MIDI_EVENT = 'Kmid',
	K_MIDI_MUTE = 'Kmut'
};

class KMidiThrough
  : public BMidiLocalConsumer
{
  public:
	KMidiThrough(const BMessenger& messenger, const char* name = NULL, BMidiLocalProducer* producer = NULL);

	virtual void Data(uchar *data, size_t length, bool atomic, bigtime_t time);

	virtual	void NoteOff(uchar channel, uchar note, uchar velocity, bigtime_t time);
	virtual	void NoteOn(uchar channel, uchar note, uchar velocity, bigtime_t time);
	virtual	void KeyPressure(uchar channel, uchar note, uchar pressure, bigtime_t time);
	virtual	void ControlChange(uchar channel, uchar controlNumber, uchar controlValue, bigtime_t time);
	virtual	void ProgramChange(uchar channel, uchar programNumber, bigtime_t time);
	virtual	void ChannelPressure(uchar channel, uchar pressure, bigtime_t time);
	virtual	void PitchBend(uchar channel, uchar lsb, uchar msb, bigtime_t time);
	virtual	void SystemExclusive(void* data, size_t dataLength, bigtime_t time);
	virtual	void SystemCommon(uchar statusByte, uchar data1, uchar data2, bigtime_t time);
	virtual	void SystemRealTime(uchar statusByte, bigtime_t time);
	virtual	void TempoChange(int32 bpm, bigtime_t time);
	virtual void AllNotesOff(bool ControlOnly, bigtime_t time);

//	const BMidiLocalProducer* Producer();
	status_t SetProducer(BMidiLocalProducer* producer);

  protected:
	virtual ~KMidiThrough();

  private:
//	KMidiThrough();	//	Not impremented.
	KMidiThrough(const KMidiThrough&);	//	Not impremented.
	KMidiThrough& operator=(const KMidiThrough&);	//	Not impremented.

	BLocker mLock;
	BMidiLocalProducer* mProducer;
	BMessenger mMessenger;
};

#endif

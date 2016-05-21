//	File: KMidiThrough.cc

#include "KMidiThrough.h"

#include <Debug.h>
#include <MidiProducer.h>


KMidiThrough::KMidiThrough(const BMessenger& messenger, const char* name, BMidiLocalProducer* producer)
  : BMidiLocalConsumer(name),
	mLock(true),
	mProducer(producer),
	mMessenger(messenger)
{
	if (mProducer != NULL) {
		status_t aErr = B_OK;

		aErr = mProducer->Acquire();
		if (aErr != B_OK)
			mProducer = NULL;
	}
}


KMidiThrough::~KMidiThrough()
{
	if (mProducer != NULL)
		mProducer->Release();
}


void
KMidiThrough::Data(uchar *data, size_t length, bool atomic, bigtime_t time)
{
	if (mProducer == NULL)
		return;

	BMidiLocalConsumer::Data(data, length, atomic, time);
//	mProducer->SprayData(data, length, atomic, time);
}


void
KMidiThrough::NoteOff(uchar channel, uchar note, uchar velocity, bigtime_t time)
{
	mProducer->SprayNoteOff(channel, note, velocity, time);
}


void
KMidiThrough::NoteOn(uchar channel, uchar note, uchar velocity, bigtime_t time)
{
	mProducer->SprayNoteOn(channel, note, velocity, time);
}


void
KMidiThrough::KeyPressure(uchar channel, uchar note, uchar pressure, bigtime_t time)
{
	mProducer->SprayKeyPressure(channel, note, pressure, time);
}


void
KMidiThrough::ControlChange(uchar channel, uchar controlNumber, uchar controlValue, bigtime_t time)
{
	mProducer->SprayControlChange(channel, controlNumber, controlValue, time);
}


void
KMidiThrough::ProgramChange(uchar channel, uchar programNumber, bigtime_t time)
{
	mProducer->SprayProgramChange(channel, programNumber, time);
}


void
KMidiThrough::ChannelPressure(uchar channel, uchar pressure, bigtime_t time)
{
	mProducer->SprayChannelPressure(channel, pressure, time);
}


void
KMidiThrough::PitchBend(uchar channel, uchar lsb, uchar msb, bigtime_t time)
{
	mProducer->SprayPitchBend(channel, lsb, msb, time);
}


void
KMidiThrough::SystemExclusive(void* data, size_t dataLength, bigtime_t time)
{
	uint8* aData = (uint8*)data;

#if DEBUG
	for (size_t i = 0; i < dataLength; i++)
		PRINT((" %02X", aData[i]));
	PRINT(("\n"));
#endif

//	0x7D, uint8(aChannel), 'K', 'T', 0x01, uint8(aValue)

	if ((dataLength == 6) && (aData[0] == 0x7D) && (aData[2] == 'K') && (aData[3] == 'T')) {
		switch (aData[4]) {
		  case 0x01:
			{
				uint8 aChannel = aData[1];
				uint8 aValue = aData[5];
				BMessage aMessage(K_MIDI_MUTE);
				aMessage.AddInt8("gopherwood:channel", aChannel);
				aMessage.AddInt8("gopherwood:value", aValue);
				mMessenger.SendMessage(&aMessage);
			}
			break;
		  default:
			break;
		}
	} else
		mProducer->SpraySystemExclusive(data, dataLength, time);
}


void
KMidiThrough::SystemCommon(uchar statusByte, uchar data1, uchar data2, bigtime_t time)
{
	mProducer->SpraySystemCommon(statusByte, data1, data2, time);
}


void
KMidiThrough::SystemRealTime(uchar statusByte, bigtime_t time)
{
	mProducer->SpraySystemRealTime(statusByte, time);
}


void
KMidiThrough::TempoChange(int32 bpm, bigtime_t time)
{
	mProducer->SprayTempoChange(bpm, time);
}


void
KMidiThrough::AllNotesOff(bool ControlOnly, bigtime_t time)
{
	for (uchar aChannel = 0; aChannel < 16; aChannel++)
		mProducer->SprayControlChange(aChannel, B_ALL_NOTES_OFF, 0, time);

	if (ControlOnly)
		return;

	for (uchar aChannel = 0; aChannel < 16; aChannel++)
		for (uchar aNote = 0; aNote < 128; aNote++)
			mProducer->SprayNoteOff(aChannel, aNote, 0x40, time);
}


//const BMidiLocalProducer*
//KMidiThrough::Producer()
//{
//	return mProducer;
//}


status_t
KMidiThrough::SetProducer(BMidiLocalProducer* producer)
{
	status_t aErr = B_OK;

	if (mLock.Lock()) {
		aErr = producer->Acquire();
		if (aErr == B_OK) {
			if (mProducer != NULL)
				mProducer->Release();

			mProducer = producer;
		}
		mLock.Unlock();
	}

	return aErr;
}

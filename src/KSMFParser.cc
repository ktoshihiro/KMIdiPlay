//======================================================================
//	File: KSMFParser.cc
//----------------------------------------------------------------------
//	Copyright © 1999 KISAKA Toshihiro.
//	All rights reserved.
//		noah@gopherwood.com
//		kisaka@pochi.iijnet.or.jp
//======================================================================

#include "KSMFParser.h"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
#include <ByteOrder.h>
#include <Errors.h>
#include <Midi2Defs.h>
#include <OS.h>
#include <stdexcept>
#include <cstdio>

#include "KMidiStorage.h"

enum {
	K_SMF_HEADER	= 'MThd',
	K_SMF_TRACK		= 'MTrk'
} smf_chunk_type;

typedef struct {
	uint32 type;
	uint32 length;
	uint8* data;
} smf_chunk;


//======================================================================
#pragma mark === class KSMFParser ===
//======================================================================

//----------------------------------------------------------------------

//----------------------------------------------------------------------

KSMFParser::KSMFParser()
{
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

KSMFParser::~KSMFParser()
{
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

status_t
KSMFParser::Load(const char* path, KMidiStorage* storage)
{
	ifstream aFile;

	aFile.open(path);
	if (!aFile)
		return B_ENTRY_NOT_FOUND;

	smf_chunk aChunk;

	// read header chunk
	try {
		aFile.read((char*)&aChunk.type, 4);
		if (!aFile) throw 1;
		aChunk.type = B_BENDIAN_TO_HOST_INT32(aChunk.type);
		if (aChunk.type != K_SMF_HEADER)
			throw logic_error("It isn't SMF.");

		aFile.read((char*)&aChunk.length, 4);
		if (!aFile) throw 2;
		aChunk.length = B_BENDIAN_TO_HOST_INT32(aChunk.length);

		aChunk.data = new uint8[aChunk.length];
		aFile.read((char*)aChunk.data, aChunk.length);
		if (!aFile) {
			delete[] aChunk.data;
			throw 3;
		}
	} catch (int& n) {
//		cerr << n << endl;
		return B_ERROR;
	} catch (exception& e) {
//		cerr << e.what() << endl;
		fprintf(stderr, "%s\n", e.what());
		return B_ERROR;
	}

	char aFormat[64];
	sprintf(aFormat, "Format %d", B_BENDIAN_TO_HOST_INT16(*(int16*)(aChunk.data)));
	storage->SetFormat(aFormat);

	uint32 aNumTracks;
	aNumTracks = B_BENDIAN_TO_HOST_INT16(*(int16*)(aChunk.data + 2));
	storage->SetNumTracks(aNumTracks);

	int16 aTimeFormat;
	aTimeFormat = B_BENDIAN_TO_HOST_INT16(*(int16*)(aChunk.data + 4));

	storage->SetTimeFormat(aTimeFormat);

#if DEBUG
	cout << path << '\n'
		 << '\t' << "format " << B_BENDIAN_TO_HOST_INT16(*(uint16*)aChunk.data) << '\n'
		 << '\t' << "chunk num: " << B_BENDIAN_TO_HOST_INT16(*(uint16*)(aChunk.data + 2)) << endl;

	if (aTimeFormat < 0) {
		cout << '\t' << "time format: SMTPE\n" << endl;
	} else {
		cout << '\t' << "time format: " << aTimeFormat << " ticks/quarter note" << endl;
	}
#endif


	// read other chunks
	vector<smf_chunk> chunks;

	while (true) {
		try {
			aFile.read((char*)&aChunk.type, 4);
			if (!aFile) throw 1;
			aChunk.type = B_BENDIAN_TO_HOST_INT32(aChunk.type);
			if (aChunk.type != K_SMF_TRACK)
				throw logic_error("It isn't track chunk.");

			aFile.read((char*)&aChunk.length, 4);
			if (!aFile) throw 2;
			aChunk.length = B_BENDIAN_TO_HOST_INT32(aChunk.length);

			aChunk.data = new uint8[aChunk.length];
			aFile.read((char*)aChunk.data, aChunk.length);
			if (!aFile) {
				delete[] aChunk.data;
				throw 3;
			}

			chunks.push_back(aChunk);
		} catch (int& n) {
//			cerr << n << endl;
			break;
		} catch (exception& e) {
//			cerr << e.what() << endl;
			fprintf(stderr, "%s\n", e.what());
			break;
		}
	}

	storage->SetLoadedTracks(chunks.size());

	{
		int i = 1;
		vector<smf_chunk>::const_iterator iter;
		for (iter = chunks.begin(); iter != chunks.end(); iter++) {
			cout << i++ << '\t';
			switch ((*iter).type) {
			  case K_SMF_HEADER:
				cout << "Type: " << "Header" << '\t';
				cout << "Format: " << B_BENDIAN_TO_HOST_INT16(*(uint16*)(*iter).data) << '\t';
				cout << "Tracks: " << B_BENDIAN_TO_HOST_INT16(*(uint16*)((*iter).data + 2)) << endl;
				break;
			  case K_SMF_TRACK:
				cout << "Type: " << "Track" << '\t';
				cout << "Length: " << (*iter).length << endl;
				break;
			  default:
				cout << "Type: " << "unknown" << '\t';
				cout << "Length: " << (*iter).length << endl;
				break;
			}
		}
	}

	{
		int i = 1;
		vector<smf_chunk>::const_iterator iter;
		for (iter = chunks.begin(); iter != chunks.end(); iter++) {
			if ((*iter).type == K_SMF_TRACK) {
				cout << "***** Track " << i++ << " *****" << endl;
				LoadTrack((*iter).data, (*iter).length, storage);
			}
		}
	}

	{
		vector<smf_chunk>::const_iterator iter;
		for (iter = chunks.begin(); iter != chunks.end(); iter++) {
			delete[] (*iter).data;
		}
	}

	return B_OK;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

void
KSMFParser::LoadTrack(const uint8* data, const uint32 length, KMidiStorage* storage)
{
	uint i = 0;
	uint32 aLength;

	uint32 aTime = 0;
	uint32 aDelta = 0;
	uint32 aStatus = 0;
	uint32 aTempo = 500000;
//	uint32 aTempoMM = 60000000 / aTempo;

	bool eot = false;

	while ((i < length) && (!eot)) {
		aLength = length - i;
		aDelta = ReadDelta(&data[i], &aLength);
		aTime += aDelta;
		i += aLength;

		if ((data[i] & 0x80) != 0) {
			aStatus = data[i++];
		}

		switch (aStatus & 0xF0) {
		  case B_NOTE_OFF:
		  case B_NOTE_ON:
		  case B_KEY_PRESSURE:
		  case B_CONTROL_CHANGE:
		  case B_PITCH_BEND:
#if DEBUG
			cout << aTime << ": " << hex << (aStatus & 0xF0) << ", " << hex << (aStatus & 0x0F) << ", " << hex << (uint32)data[i] << ", " << hex << (uint32)data[i + 1] << dec << endl;
#endif
			storage->HandleMidiEvent(aDelta, aTime, aStatus & 0xF0, aStatus & 0x0F, data[i], data[i + 1]);
			i += 2;
			break;
		  case B_PROGRAM_CHANGE:
		  case B_CHANNEL_PRESSURE:
#if DEBUG
			cout << aTime << ": " << hex << (aStatus & 0xF0) << ", " << hex << (aStatus & 0x0F) << ", " << hex << (uint32)data[i] << dec << endl;
#endif
			storage->HandleMidiEvent(aDelta, aTime, aStatus & 0xF0, aStatus & 0x0F, data[i]);
			i += 1;
			break;
		  case 0xF0:
			{
				uint32 aDataLength = 0;
				switch (aStatus) {
				  case 0xF0:
					{
#if DEBUG
						cout << aTime << ": F0 status Sys Ex" << endl;
#endif
						aLength = length - i;
						aDataLength = ReadDelta(&data[i], &aLength);
						i += aLength;
						uint8* aData = new uint8[aDataLength];
						memcpy(aData, data + i, aDataLength);
						i += aDataLength;
//						storage->HandleSystemExclusive(aDelta, aTime, 0xF0, aLength, aData);
						storage->HandleSystemExclusive(aDelta, aTime, 0xF0, aDataLength, aData);
/*
						aLength = length - i;
						aDataLength = ReadDelta(&data[i], &aLength);
						i += aLength;
						cout << "\t" << hex;
						for (uint j = 0; j < aDataLength; j++)
							cout << (int)data[i++] << ' ';
						cout << dec << endl;
*/
					}
					break;
				  case 0xF7:
					{
#if DEBUG
						cout << aTime << ": F7 status Sys Ex" << endl;
#endif
						aLength = length - i;
						aDataLength = ReadDelta(&data[i], &aLength);
						i += aLength;
						uint8* aData = new uint8[aDataLength];
						memcpy(aData, data + i, aDataLength);
						i += aDataLength;
//						storage->HandleSystemExclusive(aDelta, aTime, 0xF7, aLength, aData);
						storage->HandleSystemExclusive(aDelta, aTime, 0xF7, aDataLength, aData);
/*
						aLength = length - i;
						aDataLength = ReadDelta(&data[i], &aLength);
						i += aLength;
						cout << "\t" << hex;
						for (uint j = 0; j < aDataLength; j++)
							cout << (int)data[i++] << ' ';
						cout << dec << endl;
*/
					}
					break;
				  case 0xFF:
					{
						switch (data[i++]) {
						  case 0x00:
#if DEBUG
							cout << aTime << ": Meta Event (Sequence Number)" << endl;
#endif
							aLength = length - i;
							aDataLength = ReadDelta(&data[i], &aLength);
							i += aLength;
							i += aDataLength;
							break;
						  case 0x01:
#if DEBUG
							cout << aTime << ": Meta Event (Text Event)" << endl;
#endif
							aLength = length - i;
							aDataLength = ReadDelta(&data[i], &aLength);
							i += aLength;
							cout << "\t\"";
							for (uint j = 0; j < aDataLength; j++)
								cout << (uint8)data[i++];
							cout << '"' << endl;
							break;
						  case 0x02:
#if DEBUG
							cout << aTime << ": Meta Event (Copyright)" << endl;
#endif
							aLength = length - i;
							aDataLength = ReadDelta(&data[i], &aLength);
							i += aLength;
							i += aDataLength;
							break;
						  case 0x03:
#if DEBUG
							cout << aTime << ": Meta Event (Sequence/Track Name)" << endl;
#endif
							aLength = length - i;
							aDataLength = ReadDelta(&data[i], &aLength);
							i += aLength;
							cout << "\t\"";
							for (uint j = 0; j < aDataLength; j++)
								cout << data[i++];
							cout << '"' << endl;
							break;
						  case 0x04:
#if DEBUG
							cout << aTime << ": Meta Event (Instrument Name)" << endl;
#endif
							aLength = length - i;
							aDataLength = ReadDelta(&data[i], &aLength);
							i += aLength;
							i += aDataLength;
							break;
						  case 0x05:
#if DEBUG
							cout << aTime << ": Meta Event (Lylic)" << endl;
#endif
							aLength = length - i;
							aDataLength = ReadDelta(&data[i], &aLength);
							i += aLength;
							cout << "\t\"";
							for (uint j = 0; j < aDataLength; j++)
								cout << data[i++];
							cout << '"' << endl;
							break;
						  case 0x06:
#if DEBUG
							cout << aTime << ": Meta Event (Marker)" << endl;
#endif
							aLength = length - i;
							aDataLength = ReadDelta(&data[i], &aLength);
							i += aLength;
							cout << "\t\"";
							for (uint j = 0; j < aDataLength; j++)
								cout << data[i++];
							cout << '"' << endl;
							break;
						  case 0x07:
#if DEBUG
							cout << aTime << ": Meta Event (Cue Point)" << endl;
#endif
							aLength = length - i;
							aDataLength = ReadDelta(&data[i], &aLength);
							i += aLength;
							i += aDataLength;
							break;

						  case 0x20:
#if DEBUG
							cout << aTime << ": Meta Event (MIDI Channel Prefix)" << endl;
#endif
							aLength = length - i;
							aDataLength = ReadDelta(&data[i], &aLength);
							i += aLength;
							i += aDataLength;
							break;
						  case 0x2F:
							{
#if DEBUG
								cout << aTime << ": meta event (end Of track)" << endl;
#endif
								storage->HandleMetaEvent(aDelta, aTime, 0x2F, 0, NULL);
								aLength = length - i;
								aDataLength = ReadDelta(&data[i], &aLength);
								i += aLength;
								i += aDataLength;
								eot = true;

/*								aLength = length - i;
								aDataLength = ReadDelta(&data[i], &aLength);
								i += aLength;
								i += aDataLength;
								eot = true;
*/
							}
							break;

						  case 0x51:
							{
#if DEBUG
								cout << aTime << ": meta event (set tempo)" << endl;
#endif
								aLength = length - i;
								aDataLength = ReadDelta(&data[i], &aLength);
								i += aLength;
								aTempo = 0;
								for (uint j = 0; j < aDataLength; j++)
									aTempo = (aTempo << 8) + data[i++];
								storage->HandleTempoChange(aDelta, aTime, aTempo);

/*							aLength = length - i;
							aDataLength = ReadDelta(&data[i], &aLength);
							i += aLength;
							aTempo = 0;
							for (uint j = 0; j < aDataLength; j++)
								aTempo = (aTempo << 8) + data[i++];
							aTempoMM = 60000000 / aTempo;
							cout << '\t' << aTempoMM << " (" << aTempo << " us)" << endl;
*/
							}
							break;
						  case 0x54:
#if DEBUG
							cout << aTime << ": Meta Event (SMPTE Offset)" << endl;
#endif
							aLength = length - i;
							aDataLength = ReadDelta(&data[i], &aLength);
							i += aLength;
							i += aDataLength;
							break;
						  case 0x58:
#if DEBUG
							cout << aTime << ": meta event (time signature)" << endl;
#endif
							aLength = length - i;
							aDataLength = ReadDelta(&data[i], &aLength);
							i += aLength;
							if (aDataLength != 4)
								i += aDataLength;
							else {
								cout << '\t' << (int)data[i] << '/' << (2 << data[i + 1]) << endl;
								i += aDataLength;
							}
							break;
						  case 0x59:
#if DEBUG
							cout << aTime << ": Meta Event (Key)" << endl;
#endif
							aLength = length - i;
							aDataLength = ReadDelta(&data[i], &aLength);
							i += aLength;
							i += aDataLength;
							break;

						  case 0x7F:
#if DEBUG
							cout << aTime << ": Meta Event (Sequencer Specific)" << endl;
#endif
							aLength = length - i;
							aDataLength = ReadDelta(&data[i], &aLength);
							i += aLength;
							i += aDataLength;
							break;

						  default:
							cout << aTime << ": Meta Event (Unknown 0x" << hex << (int)data[i - 1] << dec << ")" << endl;
							aLength = length - i;
							aDataLength = ReadDelta(&data[i], &aLength);
							i += aLength;
							cout << "\t" << hex;
							for (uint j = 0; j < aDataLength; j++)
								cout << (int)data[i++] << ' ';
							cout << dec << endl;
							break;
						}
					}
					break;
				  default:
					debugger("unknown event!");
					break;
				}
			}
			break;
		  default:
			debugger("why reach here?");
			break;
		}
	}
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------

uint32
KSMFParser::ReadDelta(const uint8* data, uint32* length)
{
	if ((data == NULL) || (*length == 0))
		return 0;

	uint32 aResult = 0;
	uint32 i = 0;
	uint8 val;

	do {
		val = data[i++];
		aResult = (aResult << 7) | (val & 0x7F);
	} while ((val & 0x80) && (i < *length));

	*length = i;

	return aResult;
}

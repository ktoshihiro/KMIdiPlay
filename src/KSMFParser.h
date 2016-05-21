//======================================================================
//	File: KSMFParser.h
//----------------------------------------------------------------------
//	Copyright © 1999 KISAKA Toshihiro.
//	All rights reserved.
//		noah@gopherwood.com
//		kisaka@pochi.iijnet.or.jp
//======================================================================

#ifndef __KSMFParser_h__
#define __KSMFParser_h__

#include <SupportDefs.h>

//	Forward Declarations
class KMidiStorage;


//======================================================================
#pragma mark === class KSMFParser ===
//======================================================================

class KSMFParser {
  public:
	KSMFParser();
	virtual ~KSMFParser();

	status_t Load(const char* path, KMidiStorage* storage);

  protected:
  private:
//	KSMFParser();	//	Not impremented
	KSMFParser(const KSMFParser&);	//	Not impremented
	KSMFParser& operator=(const KSMFParser&);	//	Not impremented

	uint32 ReadDelta(const uint8* data, uint32* length);
	void LoadTrack(const uint8* data, const uint32 length, KMidiStorage* storage);
};

#endif

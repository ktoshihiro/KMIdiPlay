#include <cstring>
#include <Application.h>
#include <Bitmap.h>
#include <BitmapStream.h>
#include <File.h>
#include <TranslatorRoster.h>
#include "Bitmaps.h"

uint32
find_constant(BTranslatorRoster *roster, const char *mime)
{
  translator_id *translators;
  int32 num_translators;

  roster->GetAllTranslators(&translators, &num_translators);

  for (int32 i=0; i < num_translators; i++) {
    const translation_format* fmts;
    int32 num_fmts;

    roster->GetOutputFormats(translators[i], &fmts, &num_fmts);

    for (int32 j=0; j < num_fmts; j++) {
      if (!strcasecmp(fmts[j].MIME, mime))
	return fmts[j].type;
    }
  }
  return 0;
}


int
main()
{
	status_t aErr = B_OK;

	BApplication aApp("application/x-vnd.gopherwood-bitmaps");
	BTranslatorRoster* aRoster = BTranslatorRoster::Default();

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, 15, 10), B_CMAP8);
		memcpy(aBitmap->Bits(), eject16x11_raw, 16 * 11);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("eject16x11_raw", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, 15, 9), B_CMAP8);
		memcpy(aBitmap->Bits(), pause16x10_raw, 16 * 10);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("pause16x10_raw", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, 15, 9), B_CMAP8);
		memcpy(aBitmap->Bits(), play16x10_raw, 16 * 10);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("play16x10_raw", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, 23, 7), B_CMAP8);
		memcpy(aBitmap->Bits(), skipleft24x8_raw, 24 * 8);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("skipleft24x8_raw", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}



	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, 95, 15), B_CMAP8);
		memcpy(aBitmap->Bits(), LCDLarge96x16_raw, 95 * 15);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("LCDLarge96x16_raw", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}



	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kPlayPauseBitmapWidth - 1, kPlayPauseBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kPlayButtonBitmapBits, kPlayPauseBitmapWidth * kPlayPauseBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kPlayButtonBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kPlayPauseBitmapWidth - 1, kPlayPauseBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kDisabledPlayButtonBitmapBits, kPlayPauseBitmapWidth * kPlayPauseBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kDisabledPlayButtonBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kPlayPauseBitmapWidth - 1, kPlayPauseBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kPressedPlayButtonBitmapBits, kPlayPauseBitmapWidth * kPlayPauseBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kPressedPlayButtonBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kPlayPauseBitmapWidth - 1, kPlayPauseBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kPlayingPlayButtonBitmapBits, kPlayPauseBitmapWidth * kPlayPauseBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kPlayingPlayButtonBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kPlayPauseBitmapWidth - 1, kPlayPauseBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kPressedPlayingPlayButtonBitmapBits, kPlayPauseBitmapWidth * kPlayPauseBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kPressedPlayingPlayButtonBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kPlayPauseBitmapWidth - 1, kPlayPauseBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kPausedPlayButtonBitmapBits, kPlayPauseBitmapWidth * kPlayPauseBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kPausedPlayButtonBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kPlayPauseBitmapWidth - 1, kPlayPauseBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kPressedPausedPlayButtonBitmapBits, kPlayPauseBitmapWidth * kPlayPauseBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kPressedPausedPlayButtonBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}


	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kStopBitmapWidth - 1, kStopBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kStopButtonBitmapBits, kStopBitmapWidth * kStopBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kStopButtonBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kStopBitmapWidth - 1, kStopBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kDisabledStopButtonBitmapBits, kStopBitmapWidth * kStopBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kDisabledStopButtonBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kStopBitmapWidth - 1, kStopBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kPressedStopButtonBitmapBits, kStopBitmapWidth * kStopBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kPressedStopButtonBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}


	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kRewindBitmapWidth - 1, kRewindBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kRewindBitmapBits, kRewindBitmapWidth * kRewindBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kRewindBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kRewindBitmapWidth - 1, kRewindBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kDisabledRewindBitmapBits, kRewindBitmapWidth * kRewindBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kDisabledRewindBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kRewindBitmapWidth - 1, kRewindBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kPressedRewindBitmapBits, kRewindBitmapWidth * kRewindBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kPressedRewindBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kRewindBitmapWidth - 1, kRewindBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kSkipBackBitmapBits, kRewindBitmapWidth * kRewindBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kSkipBackBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kRewindBitmapWidth - 1, kRewindBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kDisabledSkipBackBitmapBits, kRewindBitmapWidth * kRewindBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kDisabledSkipBackBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kRewindBitmapWidth - 1, kRewindBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kPressedSkipBackBitmapBits, kRewindBitmapWidth * kRewindBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kPressedSkipBackBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kRewindBitmapWidth - 1, kRewindBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kSkippingSkipBackBitmapBits, kRewindBitmapWidth * kRewindBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kSkippingSkipBackBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kRewindBitmapWidth - 1, kRewindBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kPressedSkippingSkipBackBitmapBits, kRewindBitmapWidth * kRewindBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kPressedSkippingSkipBackBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}


	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kRewindBitmapWidth - 1, kRewindBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kSkipForwardBitmapBits, kRewindBitmapWidth * kRewindBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kSkipForwardBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kRewindBitmapWidth - 1, kRewindBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kDisabledSkipForwardBitmapBits, kRewindBitmapWidth * kRewindBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kDisabledSkipForwardBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kRewindBitmapWidth - 1, kRewindBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kPressedSkipForwardBitmapBits, kRewindBitmapWidth * kRewindBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kPressedSkipForwardBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kRewindBitmapWidth - 1, kRewindBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kSkippingSkipForwardBitmapBits, kRewindBitmapWidth * kRewindBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kSkippingSkipForwardBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}

	{
		BBitmap* aBitmap = new BBitmap(BRect(0, 0, kRewindBitmapWidth - 1, kRewindBitmapHeight - 1), B_CMAP8);
		memcpy(aBitmap->Bits(), kPressedSkippingSkipForwardBitmapBits, kRewindBitmapWidth * kRewindBitmapHeight);
	
		BBitmapStream aStream(aBitmap);
		BFile aFile("kPressedSkippingSkipForwardBitmapBits", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		uint32 aType;
		aType = find_constant(aRoster, "image/x-be-archived-bitmap");
		aErr = aRoster->Translate(&aStream, NULL, NULL, &aFile, aType);
	}
}

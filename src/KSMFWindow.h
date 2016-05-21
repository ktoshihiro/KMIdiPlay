//======================================================================
//	File: KSMFWindow.h
//----------------------------------------------------------------------
//	Copyright © 1999 KISAKA Toshihiro.
//	All rights reserved.
//		noah@gopherwood.com
//		kisaka@pochi.iijnet.or.jp
//======================================================================

#ifndef __KSMFWindow_h__
#define __KSMFWindow_h__

#include <Window.h>

//	System Headers
#include <vector>
#include <list>
#include <map>
#include <Locker.h>

//	Forward Declarations
struct entry_ref;
class BMenuBar;
class KSMFProducer;


enum {
	K_MIDI_START = 'strt',
	K_MIDI_STOP = 'stop',
	K_MIDI_REGISTER = 'rgst',
	K_OPEN_REQUESTED = 'open',
	K_SMF_LOAD = 'smfl'
};

enum ksmf_status {
	K_LOAD,
	K_STOP,
	K_PLAY,
	K_PAUSE
};


//======================================================================
#pragma mark === class KSMFWindow ===
//======================================================================

class KSMFWindow
  : public BWindow
{
  public:
	KSMFWindow(BRect frame);
	virtual ~KSMFWindow();

	virtual void MenusBeginning();
	virtual void MenusEnded();
	virtual void MessageReceived(BMessage* msg);
	virtual bool QuitRequested();

	void UpdateMenus();
	void NextFile();

  protected:
	virtual void RefsReceived(BMessage* msg);

  private:
	KSMFWindow();
	KSMFWindow(const KSMFWindow&);
	KSMFWindow& operator=(const KSMFWindow&);

	BMenuBar* CreateMenuBar();

//	BMenu* CreatePortsMenu(const char* name = NULL);
//	void CreatePortsMenu(BMenu* menu);

	void UpdateControlMenu();
	void UpdatePortsMenu();
	void UpdateFilesPopUpMenu();

	BPopUpMenu* CreateFilesMenu();

	status_t Load(const entry_ref& ref);
	status_t Load();
	void Start();
	void Stop();

	void UpdateInfo(BMessage* msg);
//	void HandleMidiEvent(BMessage* msg);

	KSMFProducer* mSMFProducer;

	typedef vector<entry_ref> midi_files;
	midi_files mFiles;
	int32 mFileIndex;

	ksmf_status mStatus;

	BLocker mMenuLock;
};

#endif

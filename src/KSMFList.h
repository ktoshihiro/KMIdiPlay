//	KSMFList.h

//	System Headers
#include <vector>


class KSMFList
{
  public:
	KSMFList();
	virtual ~KSMFList();

  protected:

  private:
//	KSMFList();	//	Not impremented.
	KSMFList(const KSMFList&);	//	Not impremented.
	KSMFList& operator=(const KSMFList&);	//	Not impremented.

	typedef vector<entry_ref> midi_files;
	midi_files mFiles;
};

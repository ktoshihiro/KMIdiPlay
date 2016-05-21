//======================================================================
//	main.cc
//----------------------------------------------------------------------
//	Copyright © 1999 KISAKA Toshihiro.
//	All rights reserved.
//		noah@gopherwood.com
//		kisaka@pochi.iijnet.or.jp
//======================================================================

//	System Headers
#include <cstdlib>

//	Project Headers
#include "KSMFApp.h"
#include "SMFProducer.h"


//----------------------------------------------------------------------

//----------------------------------------------------------------------

int
main()
{
	KSMFApp aApp(kSignature);

	aApp.Run();

	return EXIT_SUCCESS;
}

/* ------------------------------------------------------------------ 

	Title: DPMarker

	Description:  Marking services for Data+ data

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		19 July, 2000
			Split off from DPRegression
		
------------------------------------------------------------------ */
#include "DPGlobals.h"
#include "DPMarkWindow.h"

#include <AppKit.h>
#include <StorageKit.h>

// -------------------------------------------------------------------
//	* Main app header
// -------------------------------------------------------------------

class DPMarker : public BApplication {

	public :
		
		DPMarker();
		
	virtual void
		MessageReceived(BMessage* theMesg);
};
// -------------------------------------------------------------------
//	* MAIN Program
// -------------------------------------------------------------------
int
main()
{
	DPMarker theApp;

	theApp.Run();

	return(0);
}
// -------------------------------------------------------------------
//	* CONSTRUCTORS and DESTRUCTORS
// -------------------------------------------------------------------
DPMarker::DPMarker() : BApplication(data_plus_marker_sig)
{
}

// -------------------------------------------------------------------
// *  Public Methods             
// -------------------------------------------------------------------
void
DPMarker::MessageReceived(BMessage* theMesg)
{
	switch(theMesg->what) {
	
		case mesg_mark :
			new DPMarkWindow(theMesg,BRect(100,100,150,150));
			break;
			
		default :
			BApplication::MessageReceived(theMesg);
			
	}
	
}
// -------------------------------------------------------------------

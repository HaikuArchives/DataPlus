/* ------------------------------------------------------------------ 

	Title: DPGpSelect

	Description:  Dialog for selecting group size

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		19 July, 2000
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DPGpSelect
#define _DPGpSelect

#include <InterfaceKit.h>

const uint32 mesg_gpSize	= 'Gsiz';	// Data is sent in this message

class DPGpSelect : public BWindow {

	public :
	
	DPGpSelect(float oldSize,
					 BRect frame, 
					 const char* name = "Set Group Size",
					 window_look look = B_TITLED_WINDOW_LOOK,
					 window_feel feel = B_NORMAL_WINDOW_FEEL,
					 uint32 flags		= B_NOT_RESIZABLE | B_NOT_ZOOMABLE);
					 
	void
		MessageReceived(BMessage* theMesg);		

	protected :	// Data
	BTextControl*	mControl;
	BButton*			mButton;
		
};

#endif


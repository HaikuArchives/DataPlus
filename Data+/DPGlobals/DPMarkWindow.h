/* ------------------------------------------------------------------ 

	Title: DPMarkWindow

	Description:  The window where selected data may be marked

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		07 October, 1999
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DPMarkWindow
#define _DPMarkWindow

#include "DPDataList.h"
#include "WColorControl.h"
#include <InterfaceKit.h>

class DPMarkWindow : public BWindow {

	public :
	
	DPMarkWindow(BMessage* theData,
					 BRect frame, 
					 const char* name = "Selected Data",
					 window_look look = B_TITLED_WINDOW_LOOK,
					 window_feel feel = B_NORMAL_WINDOW_FEEL,
					 uint32 flags		= B_NOT_RESIZABLE | B_NOT_ZOOMABLE);
					 
	void
		MessageReceived(BMessage* theMesg);		

	protected :	// Data
	
	DPDataList*		mListView;
	WColorControl*	mColorCtrl;
	BRadioButton*	mMark;
	BRadioButton*	mSuppress;
	BRadioButton*	mDelete;
	BButton*			mButton;
		
};

#endif


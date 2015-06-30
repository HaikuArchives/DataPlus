/* ------------------------------------------------------------------ 

	Title: DataPlus

	Description:  An application for graphical analysis of scientific
		data.

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		20 September, 1999
			Much rearranging
		14 August, 1999
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DataPlus
#define _DataPlus

#include "DPDataWindow.h"
#include <AppKit.h>

class DataPlus : public BApplication {

	public:
		DataPlus();
		
	virtual
		~DataPlus();
		
	void
		AboutRequested();
	
	void
		MessageReceived(BMessage* theMesg);
		
	virtual void
		RefsReceived(BMessage* theMesg);
		
	protected :	// Data
	
	DPDataWindow*	mDataWindow;
};

#endif


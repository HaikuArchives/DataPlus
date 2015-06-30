/* ------------------------------------------------------------------ 

	Title: DRRegrDisplay

	Description:  The window which shows tab views of the regression
		data and which allows setting of the plot marks and colors

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		11 November, 1999
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DRRegrDisplay
#define _DRRegrDisplay

#include "DRRegrData.h"

class DRRegrDisplay : public BWindow {

	public:
		DRRegrDisplay(BRect frame,
						const char* title = "Regr Data",
						window_look look = B_TITLED_WINDOW_LOOK,
						window_feel feel = B_NORMAL_WINDOW_FEEL,
						uint32 flags = B_NOT_CLOSABLE | B_NOT_ZOOMABLE | B_NOT_RESIZABLE); 
		
	virtual
		~DRRegrDisplay();
		
	void
		AddDisplay(DRRegrData* theData);
		
	BView*
		GetView(int32 tabNum); 
		
	virtual void
		MessageReceived(BMessage* theMessage);
				
	void
		RefreshDisplays();
		
	protected:	// Data
	
	BTabView*	mTab;
	bool		mFirstDisp;
};

#endif


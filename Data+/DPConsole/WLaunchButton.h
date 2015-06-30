/* ------------------------------------------------------------------ 

	Title: WLaunchButton

	Description:  
		An icon button for launching applications

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		10 September, 1999
			Created
		
------------------------------------------------------------------ */ 

#ifndef _WLaunchButton
#define _WLaunchButton

#include <InterfaceKit.h>

class WLaunchButton : public BControl {

	public :
	
		WLaunchButton(BRect frame, const char* appSig, BMessage* theMesg);
		
	virtual
		~WLaunchButton();
		
	void
		SetBitmapIDs(int32 outMap, int32 inMap, int32 disMap);
		
	virtual void
		SetEnabled(bool enabled);
		
	protected :	// Data
	
	virtual void
		Draw(BRect frame);
	
	virtual void
		MouseDown(BPoint thePt);
		
	virtual void
		MouseMoved(BPoint point, uint32 transit, const BMessage* theMesg);
		
	virtual void
		MouseUp(BPoint thePt);
		
	protected :	// Data
	char			mAppSig[64];
	BBitmap*		mOutMap;
	BBitmap*		mInMap;
	BBitmap*		mDisMap;
	BBitmap*		mCurrMap;
	bool			mTracking;
};
	
#endif


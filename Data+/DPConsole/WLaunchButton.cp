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

#include "WLaunchButton.h"

#include <AppKit.h>
#include <StorageKit.h>
#include <SupportKit.h>
#include "WU_AppUtils.h"

// -------------------------------------------------------------------
// *  Constructors & Destructor             
// -------------------------------------------------------------------
WLaunchButton::WLaunchButton(BRect frame, const char* appSig, BMessage* theMesg)
					:BControl(frame, appSig, NULL, theMesg, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW)
{
	::strcpy(mAppSig,appSig);
	mOutMap = NULL;
	mInMap = NULL;
	mDisMap = NULL;
	mCurrMap = NULL;
	mTracking = false;
}
		
WLaunchButton::~WLaunchButton()
{
	delete mOutMap;
	delete mInMap;
	delete mDisMap;
}
// -------------------------------------------------------------------
// *  Public Methods             
// -------------------------------------------------------------------
void
WLaunchButton::SetBitmapIDs(int32 outMap, int32 inMap, int32 disMap)
{
//	mOutMap = ResToBmp(outMap);
//	mInMap = ResToBmp(inMap);
//	mDisMap = ResToBmp(disMap);
//	mCurrMap = mOutMap;
	mOutMap = ::GetAppBitmap(outMap);
	mInMap = ::GetAppBitmap(inMap);
	mDisMap = ::GetAppBitmap(disMap);
	mCurrMap = mOutMap;
}
// -------------------------------------------------------------------
void
WLaunchButton::SetEnabled(bool enabled)
{
	if(enabled == IsEnabled()) {
		return;
	}
	BAutolock lock(Window());
	BControl::SetEnabled(enabled);
	if(enabled) {
		mCurrMap = mOutMap;
	} else {
		mCurrMap = mDisMap;
	}
	if(Window()) {
		Draw(BRect(0,0,0,0));
	}
}
// -------------------------------------------------------------------
// *  Protected Methods             
// -------------------------------------------------------------------
void
WLaunchButton::Draw(BRect frame)
{
	if(mCurrMap != NULL) {
		DrawBitmap(mCurrMap,Bounds());
		
	} else {
		SetHighColor(255,200,200);	// Light pink
		FillRect(Bounds());
	}
}
// -------------------------------------------------------------------
void
WLaunchButton::MouseDown(BPoint thePt)
{
	if(IsEnabled()) {
		mTracking = true;
		mCurrMap = mInMap;
		Window()->Lock();
		Draw(BRect(0,0,0,0));
		Window()->Unlock();
	}
}
// -------------------------------------------------------------------
void
WLaunchButton::MouseMoved(BPoint point, uint32 transit, const BMessage* theMesg)
{
	bool change = false;
	if(mTracking) {
		if(transit & B_EXITED_VIEW) {
			mCurrMap = mOutMap;
			change = true;
		// The following is a kludge, because B_ENTERED_VIEW does not work....
		} else {	// Test the buttons to see they were released
			BPoint cursor;
			uint32 buttons;
			GetMouse(&cursor,&buttons);
			if(!(buttons & B_PRIMARY_MOUSE_BUTTON)) {	// If button not pressed
				mTracking = false;
				mCurrMap = mOutMap;
				change = true;
			} else if(mCurrMap != mInMap) {
				mCurrMap = mInMap;
				change = true;
			}
		}
			
		if(change) {
			Window()->Lock();
			Draw(BRect(0,0,0,0));
			Window()->Unlock();
		}
	}
			
}
// -------------------------------------------------------------------
void
WLaunchButton::MouseUp(BPoint thePt)
{
	if(mTracking) {
		mCurrMap = mOutMap;
		Invoke();
		Window()->Lock();
		Draw(BRect(0,0,0,0));
		Window()->Unlock();
	}
}
// -------------------------------------------------------------------

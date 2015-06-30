/* ------------------------------------------------------------------ 

	Title: DRRegrView

	Description:  The master regression plot view This is a view
		which holds the regression data output panel, plus controls
		for changing the plot marks and color.

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		23 November, 1999
			Revised for single window look
		30 October, 1999
			Added controls
		14 August, 1999
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DRRegrView
#define _DRRegrView

#include "DRRegrData.h"
#include "WSRegressionView.h"
#include "WGPlotMarkMenu.h"
#include "WColorControl.h"
#include <InterfaceKit.h>

class DRRegrView : public BView{

	public:
		DRRegrView(DRRegrData*	theData,
					BRect frame 		= BRect(0,0,5,5),
					const char* title	= "dpregrview",
					uint32 mode			= B_FOLLOW_TOP | B_FOLLOW_LEFT,
					uint32 flags		= B_WILL_DRAW + 
											B_FULL_UPDATE_ON_RESIZE + 
											B_FRAME_EVENTS); 
	virtual
		~DRRegrView();

	virtual void
		GetPreferredSize(float* width, float* height);
		
	void
		MessageReceived(BMessage* theMesg);
		
	WSRegressionView*
		DisplayView() const {return mDataView;}
		
	virtual void
		Draw(BRect frame);
		
	void
		ResetDisplay();
		
	virtual void
		ResizeToPreferred();
		
	bool
		SameKey(record_key theKey) const {return theKey == mKey;}
				
	protected:	// Methods
	
	virtual void
		AttachedToWindow();
	
	protected:	// Data
	
	record_key			mKey;
	DRRegrData*			mData;
	WSRegressionView*	mDataView;
	WGPlotMarkMenu*	mPlotMark;
	WColorControl*		mColorCtrl;
	BMenuField*			mLineSelect;
	BButton*				mButton;
};

#endif


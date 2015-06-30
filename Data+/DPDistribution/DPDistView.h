/* ------------------------------------------------------------------ 

	Title: DPDistView

	Description:  The master distribution plot view This is a view
		which holds the distribution data output panel, plus controls
		for changing the distribution marks and color.

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		05 June, 2000
			Revised for distribution view
		23 November, 1999
			Revised for single window look
		30 October, 1999
			Added controls
		14 August, 1999
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DPDistView
#define _DPDistView

#include "DPDistData.h"
#include "WSDistributionView.h"
#include "WColorControl.h"
#include <InterfaceKit.h>

class DPDistView : public BView{

	public:
		DPDistView(DPDistData*	theData,
					BRect frame 		= BRect(0,0,5,5),
					const char* title	= "DPDistView",
					uint32 mode			= B_FOLLOW_TOP | B_FOLLOW_LEFT,
					uint32 flags		= B_WILL_DRAW + 
											B_FULL_UPDATE_ON_RESIZE + 
											B_FRAME_EVENTS); 
	virtual
		~DPDistView();

	virtual void
		GetPreferredSize(float* width, float* height);
		
	void
		MessageReceived(BMessage* theMesg);
		
	const DPDistData*
		Data() {return (const DPDistData*)mData;}
		
	WSDistributionView*
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
	DPDistData*			mData;
	WSDistributionView*	mDataView;
	WColorControl*		mColorCtrl;
	BButton*				mButton;
};

#endif


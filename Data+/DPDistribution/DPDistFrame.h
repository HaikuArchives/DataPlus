/* ------------------------------------------------------------------ 

	Title: DPDistFrame

	Description:  A frame for showing the sets of regression data.
	
	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		05 June, 2000
			Created
		
------------------------------------------------------------------ */
#ifndef _DPDistFrame
#define _DPDistFrame

#include "DPDistData.h"
#include "DPDistView.h"
#include <InterfaceKit.h>

class DPDistFrame : public BView {

	public :
	
		DPDistFrame(BRect frame, 	
							const char* name = "DistFrame",
							uint32 resizingMode = B_FOLLOW_TOP | B_FOLLOW_RIGHT,
							uint32 flags = B_WILL_DRAW);
							
	virtual
		~DPDistFrame();
		
	void
		AddDisplay(DPDistData* theData);
		
	
	const DPDistView*
		DisplayView(int32 viewNum) const;
		
	virtual void
		Draw(BRect frame);
		
	void
		RefreshDisplays();
		
	virtual void
		ResizeToPreferred();
		
	protected :
		BList		mData;
		
};
#endif

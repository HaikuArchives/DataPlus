/* ------------------------------------------------------------------ 

	Title: DRDisplayFrame

	Description:  A frame for showing the sets of regression data.
	
	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		23 November, 1999
			Created
		
------------------------------------------------------------------ */
#ifndef _DRDisplayFrame
#define _DRDisplayFrame

#include "DRRegrData.h"
#include "DRRegrView.h"
#include <InterfaceKit.h>

class DRDisplayFrame : public BView {

	public :
	
		DRDisplayFrame(BRect frame, 	
							const char* name = "DispFrame",
							uint32 resizingMode = B_FOLLOW_TOP | B_FOLLOW_RIGHT,
							uint32 flags = B_WILL_DRAW);
							
	virtual
		~DRDisplayFrame();
		
	void
		AddDisplay(DRRegrData* theData);
		
	
	const DRRegrView*
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

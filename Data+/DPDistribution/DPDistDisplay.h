/* ------------------------------------------------------------------ 

	Title: DPDistDisplay

	Description:  A frame for showing the sets of distribution data.
	
	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		23 November, 1999
			Created
		
------------------------------------------------------------------ */
#ifndef _DPDistDisplay
#define _DPDistDisplay

#include <InterfaceKit.h>

class DPDistDisplay : public BView {

	public :
	
		DPDistDisplay(BRect frame, 	
							const char* name = "DispFrame",
							uint32 resizingMode = B_FOLLOW_TOP | B_FOLLOW_RIGHT,
							uint32 flags = B_WILL_DRAW);
							
	virtual
		~DPDistDisplay();
		
//	void
//		AddDisplay(DPRegrData* theData);
		
	
//	const DPRegrView*
//		DisplayView(int32 viewNum) const;
		
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

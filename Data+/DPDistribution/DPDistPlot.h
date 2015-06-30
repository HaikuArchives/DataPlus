/* ------------------------------------------------------------------ 

	Title: DPDistPlot

	Description:  The distribution plotting view for Data+

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		14 April, 2000
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DPDistPlot
#define _DPDistPlot

#include "WGStdGraph.h"


class DPDistPlot : public WGStdGraph {

	public:
	
		DPDistPlot(BRect frame,  
					  const char* title	= "DPDistPlot",
					  uint32 mode			= B_FOLLOW_ALL_SIDES,
					  uint32 flags			= B_WILL_DRAW | 
					  							  B_FULL_UPDATE_ON_RESIZE | 
												  B_FRAME_EVENTS); 
	virtual	
		~DPDistPlot();
		
	void
		RefreshPlot();
		
	void
		Rescale();
		
	protected:

	virtual void
		MouseDown(BPoint thePoint);
	virtual void
		MouseMoved(BPoint thePt, uint32 transit, const BMessage* theMesg);
	virtual void
		MouseUp(BPoint thePt);
	virtual void
		Draw(BRect frame);
		
	protected :	// Data
	
	void
		DrawDistribution();
	void
		DrawPercentile();
	void
		DrawProbit();
	
	BCursor*		mCrossCurs;
	bool			mSelect;
	
};

#endif

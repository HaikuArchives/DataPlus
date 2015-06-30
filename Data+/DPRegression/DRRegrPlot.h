/* ------------------------------------------------------------------ 

	Title: DRRegrPlot

	Description:  The plotting view for Data+

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		19 August, 1999
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DRRegrPlot
#define _DRRegrPlot

#include "WGStdGraph.h"
#include "WGOdometer.h"


class DRRegrPlot : public WGStdGraph {

	public:
	
		DRRegrPlot(BRect frame,  
					  const char* title	= "DRRegrPlot",
					  uint32 mode			= B_FOLLOW_ALL_SIDES,
					  uint32 flags			= B_WILL_DRAW | 
					  							  B_FULL_UPDATE_ON_RESIZE | 
												  B_FRAME_EVENTS); 
	virtual	
		~DRRegrPlot();
		
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
	
	BCursor*		mCrossCurs;
	bool			mSelect;
	
};

#endif

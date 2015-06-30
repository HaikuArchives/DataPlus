/* ------------------------------------------------------------------ 

	Title: DPMultRegr

	Description:  Multiple-regression analysis for Data+

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		16 March, 2001
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DPMultRegr
#define _DPMultRegr

#include "DPData.h"
#include "DPMRWindow.h"
#include <AppKit.h>

class DPMultRegr : public BApplication {

	public:
		DPMultRegr();
		
	virtual
		~DPMultRegr();
		
	void
		AboutRequested();
		
	const DPData*
		DataSet(int16 setNum);
		
	int32
		NumSets() {return mData->CountItems();}
		
	void
		MessageReceived(BMessage* theMesg);
		
	protected :	// Data
	
	void
		MarkData(BMessage* theMesg);
		
	
	DPMRWindow*		mMRWindow;
	BList*			mData;
};

#endif


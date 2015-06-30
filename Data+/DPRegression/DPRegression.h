/* ------------------------------------------------------------------ 

	Title: DPRegression

	Description:  Provides regression analysis & graphing for Data+

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		29 November, 1999
			Modified for single-window
		12 September, 1999
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DPRegression
#define _DPRegression

#include "DPData.h"
#include "DRRegrWindow.h"
#include "WSRegressionView.h"
#include <AppKit.h>

class DPRegression : public BApplication {

	public:
		DPRegression();
		
	virtual
		~DPRegression();
		
	void
		AboutRequested();
		
	void
		AddDisplaySet(DPData* theSet);
		
	void
		ClearDataSets();
	
	const DPData*
		DataSet(int16 setNum);
		
	const DRRegrData*
		DisplayData(int16 setNum) {return (const DRRegrData*)mData->ItemAt(setNum);}
		
	int32
		NumSets() {return mData->CountItems();}
		
	void
		MessageReceived(BMessage* theMesg);
		
	protected :	// Data
	
	void
		MarkData(BMessage* theMesg);
		
	void
		RefsReceived(BMessage* theMesg);
	void
		SetDisplays(BMessage* theMesg);
	
	DRRegrWindow*	mRegrWindow;
	BList*			mData;
};

#endif


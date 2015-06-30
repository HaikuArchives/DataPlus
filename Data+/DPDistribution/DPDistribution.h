/* ------------------------------------------------------------------ 

	Title: DPDistribution

	Description:  Distribution analysis for Data+

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		14 April, 2000
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DPDistribution
#define _DPDistribution

#include "DPDistData.h"
#include "DPDistWindow.h"
#include <AppKit.h>

class DPDistribution : public BApplication {

	public:
		DPDistribution();
		
	virtual
		~DPDistribution();
		
	void
		AboutRequested();
		
	void
		AddDisplaySet(DPData* theData);
		
	const DPData*
		Data(int16 setNum) const;
		
	const DPDistData*
		DataSet(int16 setNum) const {return (const DPDistData*)mData->ItemAt(setNum);}
		
	EDistType
		DistType() const {return mDistType;}
		
	float 
		GroupWidth() const {return mGroupWd;}
	
	bool
		IsFitted() const {return mFitted;}
		
	int32
		NumSets() {return mData->CountItems();}
		
	virtual void
		MessageReceived(BMessage* theMesg);
		
	void
		SetDisplays(BMessage* theMesg);
		
	void
		SetDistType(EDistType theType) {mDistType = theType;}
		
	void
		SetFitFlag(bool fit) {mFitted = fit;}
		
	void
		SetGroupWidth(float gpWd) {mGroupWd = gpWd;}
		
	protected :	// Data
	
	void
		MarkData(BMessage* theMesg);
		
	void
		RefsReceived(BMessage* theMesg);
		
	
	BList*			mData;
	DPDistWindow*	mDistWindow;
	float			mGroupWd;
	EDistType		mDistType;
	bool			mFitted;
};

#endif


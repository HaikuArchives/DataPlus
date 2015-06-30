/* ------------------------------------------------------------------ 

	Title: DPDistFit

	Description:  A data fitting class for the distribution data
	
	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		24 July, 2000
			Created
		
------------------------------------------------------------------ */
#ifndef _DPDistFit
#define _DPDistFit

#include "WMFunctBase.h"
#include "WSLinearRegr.h"
#include "WMProbits.h"
#include "DPDistData.h"

class DPDistFit : public WMFunctBase {

	public :
	
		DPDistFit(DPDistData* theData);
		
	virtual float
		CalcYFromX(float x);
		
	void
		SetFitType(EDistType dType) {mDistType = dType;}
		
	protected :
	DPDistData*		mData;
	WMProbits		mPFunct;
	WSLinearRegr	mLinRegr;
	float 			mGpSize;
	float 			mLastInt;
	EDistType		mDistType;
};

#endif


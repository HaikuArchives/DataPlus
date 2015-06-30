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
#include "DPDistFit.h"

#include "DPDistribution.h"

// -------------------------------------------------------------------
//	* CONSTRUCTORS and DESTRUCTORS
// -------------------------------------------------------------------
DPDistFit::DPDistFit(DPDistData* theData)
{
	mData = theData;
	mDistType = distType_distribution;
	
	// Fit the data from about the 5 to 95th percentile
	int32 points = mData->DataPoints();
	int32 index5 = points*5/100;
	int32 index95 = points*95/100;
	for(int32 indx = index5; indx < index95; indx++) {
		float xVal;
		float yVal;
		bool marked;
		if(mData->Value(indx,xVal,marked) && mData->Percentile(indx,yVal)) {
			mLinRegr.AddData(xVal,mPFunct.XformTo(yVal));
		}
	}
	mLinRegr.CalcRegression();
	mLastInt = 10E10;
}
// -------------------------------------------------------------------
//	* PUBLIC METHODS
// ------------------------------------------------------------------ 
// NOTE that the x-values are probits 0..10 and will need to be
// converted when the Z-score (SD) is returned
float
DPDistFit::CalcYFromX(float x)
{
	if(!mLinRegr.RegressionValid()) {
		mMathErr = -1;
		return 0;	/// EXIT HERE if no regression
	}
	
	mMathErr = 0;
	float pct1, pct2,yVal;
	DPDistribution* theApp;
	
	switch(mDistType) {
	
		case distType_distribution :
			if(mLastInt > x) {	// Reset interval marker
				mLastInt = x;
				theApp = cast_as(be_app,DPDistribution);
				mGpSize = theApp->GroupWidth();
				
			} else {
				pct1 = mPFunct.XformFrom(mLinRegr.CalcYFromX(x));
				pct2 = mPFunct.XformFrom(mLinRegr.CalcYFromX(mLastInt));
				yVal = (mGpSize/(x - mLastInt))*mData->DataPoints()*(pct1 - pct2)/100.0;
				mLastInt = x;
				return yVal;
			}
			break;
		
		case distType_percentile :
			return mPFunct.XformFrom(mLinRegr.CalcYFromX(x));
			break;
			
		case distType_probit :
			return mLinRegr.CalcYFromX(x) - 5.0;
			
		default :
			;	// No default case	
		}
		
	mMathErr = -1;
	return 0;
}

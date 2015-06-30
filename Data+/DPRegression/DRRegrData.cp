/* ------------------------------------------------------------------ 

	Title: DRRegrData

	Description:  A wrapper class for displaying statistics
	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		22 September, 1999
			Created
		
------------------------------------------------------------------ */
#include "DRRegrData.h"
#include "UStringFmt_Utils.h"

// -------------------------------------------------------------------
//	* CONSTRUCTORS and DESTRUCTORS
// -------------------------------------------------------------------
DRRegrData::DRRegrData(DPData* theData) : DPStatData(theData)
{
	mPlotMark = plotMark_dot;
	mLineType = rLine_regression;
	Recalculate();
}
		

DRRegrData::~DRRegrData()
{
	// The windows delete themselves...
}
// -------------------------------------------------------------------
//	* Public Methods
// -------------------------------------------------------------------
void
DRRegrData::MarkData(BMessage* theMesg)
{
}
// -------------------------------------------------------------------
void
DRRegrData::MessageReceived(BMessage* theMesg)
{
	switch (theMesg->what) {
		
		default :
			DPStatData::MessageReceived(theMesg);
	}
}
// -------------------------------------------------------------------
WMFunctBase*
DRRegrData::PlotFunction()
{
	return (WMFunctBase*)&mRegression;
}
// -------------------------------------------------------------------
void
DRRegrData::Recalculate()
{
	mRegression.ClearData();
	float x,y;
	for(int32 n = 0; n < mData->NumRecords(); n++) {
		if(mData->FindRecord(n) && mData->XValue(x) && mData->YValue(y)) {
			if(mData->CurrentRec()->DataStatus() <= data_marked) {
				mRegression.AddData(x,y);
			}
		}
	}
	mRegression.CalcRegression();
}
// -------------------------------------------------------------------
void
DRRegrData::SwapAxes()
{
	int16 xFld = mData->XField();
	int16 yFld = mData->YField();
	int16 zFld = mData->ZField();
	mData->SetXYZFields(yFld,xFld,zFld);
	Recalculate();
}
// -------------------------------------------------------------------
void
DRRegrData::UserSelections()
{
}
// -------------------------------------------------------------------
//	* Protected Methods
// -------------------------------------------------------------------
void
DRRegrData::MakePatternForFunct()
{
}
// -------------------------------------------------------------------

/* ------------------------------------------------------------------ 

	Title: DPStatData

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
#include "DPStatData.h"

#include "DPGlobals.h"
#include "WColorControl.h"

// -------------------------------------------------------------------
//	* CONSTRUCTORS and DESTRUCTORS
// -------------------------------------------------------------------
DPStatData::DPStatData(DPData* theData)
{
	mData = theData;
	mBlendColors = false;
	mLineType = line_regression;
	mLineStyle = style_solid;
	mMarkStatus = mkstat_mark;
	mDataColor = color_black;
	mMaxColor = color_red;
	mMinColor = color_black;
}	

DPStatData::~DPStatData()
{
	delete mData;
}
// -------------------------------------------------------------------
//	* Public Methods
// -------------------------------------------------------------------
rgb_color
DPStatData::ColorForValue(float val)
{
	return color_black;
}
// -------------------------------------------------------------------
void
DPStatData::EnableColorBlend(int16 dataFld)
{
	// If the 'dataFld' is valid, max and min values are found,
	// and a color blend is set up
}
// -------------------------------------------------------------------
void
DPStatData::MessageReceived(BMessage* theMesg)
{
	BHandler::MessageReceived(theMesg);
}
// -------------------------------------------------------------------
void
DPStatData::RestorePen(BView* theView)
{
}
// -------------------------------------------------------------------
void
DPStatData::SetUpPen(BView* theView)
{
}
// -------------------------------------------------------------------
//	* Protected Methods
// -------------------------------------------------------------------
void
DPStatData::CalcColorBlend()
{
	if(!mBlendColors) {
		return;	// Don't bother until enabled
	}
}
// -------------------------------------------------------------------
void
DPStatData::MakePatternForFunct()
{
}
// -------------------------------------------------------------------

/* ------------------------------------------------------------------ 

	Title: DMRStatView

	Description:  View for showing Multiple-regression statistics
	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		18 August, 2001
			Created
		
------------------------------------------------------------------ */ 
#include "DMRStatView.h"

// -------------------------------------------------------------------
//	* CONSTRUCTORS and DESTRUCTORS
// -------------------------------------------------------------------
DMRStatView::DMRStatView(int16 xFld, int16 yFld, BRect frame,const char* name,const char* text) : BStringView(frame,name,text)
{
	mRegr.ClearData();
	SetDisplay(disp_nothing);
	mXFld = xFld;
	mYFld = yFld;
	mPrec = 3;
	mFormat = realFmt_general;
}
						
DMRStatView::~DMRStatView()
{
}
// -------------------------------------------------------------------
//	* PUBLIC METHODS
// ------------------------------------------------------------------ 
void
DMRStatView::SetDisplay(EStatDisplays theDisplay)
{
	mDispType = theDisplay;
	SetDisplayText();
}
// ------------------------------------------------------------------ 
void
DMRStatView::SetRealFormat(ERealFormat format, int16 prec)
{
	mFormat = format;
	mPrec = prec;
	SetDisplayText();
}
// -------------------------------------------------------------------
//	* PROTECTED METHODS
// ------------------------------------------------------------------ 
void
DMRStatView::SetDisplayText()
{
	const int16 max_chars = 15;
	char temp[max_chars + 1] = "----";
	
	// These are only shown in the center section
	if(mXFld == mYFld) {
		if(mRegr.SDValid()) {
		
			switch(mDispType) {

				case disp_number:
					::FormatReal(mRegr.PointsIn(),mFormat,mPrec,temp);
					break;
				
				case disp_mean:
					::FormatReal(mRegr.MeanX(),mFormat,mPrec,temp);
					break;
	
				case disp_SD:
					::FormatReal(mRegr.StdDevX(),mFormat,mPrec,temp);
					break;
				}
		} else {
			::strcpy(temp,"####");
		}
		
	// These are shown only in the top part of the matrix
	} else if(mXFld > mYFld) {
	
		if(mRegr.RegressionValid()) {
			switch(mDispType) {
	
			case disp_tValue:
				::FormatReal(mRegr.CorrelTValue(),mFormat,mPrec,temp);
				break;
	
			case disp_rValue:
				::FormatReal(mRegr.CorrelRValue(),mFormat,mPrec,temp);
				break;
	
			case disp_pValue:
				if(mRegr.ProbabilityValid()) {
					::FormatReal(mRegr.PValue(),mFormat,mPrec,temp);
				} else {
					::strcpy(temp,"####");
				}
				break;
			}
		} else {
			::strcpy(temp,"####");
		}
		
	// These are shown in all but the center section
	} else {
		if(mRegr.RegressionValid()) {
			switch(mDispType) {
				
			case disp_slope:
				::FormatReal(mRegr.RegressionSlope(),mFormat,mPrec,temp);
				break;
	
			case disp_intercept:
				::FormatReal(mRegr.RegressionIntercept(),mFormat,mPrec,temp);
				break;
	
			case disp_SDSlope:
				::FormatReal(mRegr.RegressionSDSlope(),mFormat,mPrec,temp);
				break;
	
			case disp_SDIntercept:
				::FormatReal(mRegr.RegressionSDIntercept(),mFormat,mPrec,temp);
				break;
			}
		} else {
			::strcpy(temp,"####");
		}
	}
	
	SetText(temp);
	
}
// ------------------------------------------------------------------ 

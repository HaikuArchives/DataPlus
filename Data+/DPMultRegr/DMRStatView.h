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

#ifndef _DMRStatView
#define _DMRStatView

#include "WSLinearRegr.h"
#include "URealFmt_Utils.h"
#include <InterfaceKit.h>

enum EStatDisplays {
	disp_nothing,
	disp_number,
	disp_mean,
	disp_SD,
	disp_slope,
	disp_intercept,
	disp_SDSlope,
	disp_SDIntercept,
	disp_tValue,
	disp_rValue,
	disp_pValue
};

class DMRStatView : public BStringView {

		DMRStatView(int16 xFld, int16 yFld, BRect frame = BRect(0,0,0,0),
						const char* name = "sView",
						const char* text = "---");
						
	virtual
		~DMRStatView();
		
	void
		AddData(float xVal, float yVal) {mRegr.AddData(xVal,yVal);}
		
	void
		Calculate() {mRegr.CalcRegression();}
		
	void
		ClearData() {mRegr.ClearData();}
		
	void
		SetDisplay(EStatDisplays theDisplay);
		
	void
		SetRealFormat(ERealFormat format, int16 prec);
		
	protected :	// Methods
	
	void
		SetDisplayText();
	
	
	protected :	// Data
	
	WSLinearRegr	mRegr;	
	int16				mXFld;
	int16				mYFld;
	int16				mPrec;
	EStatDisplays	mDispType;	
	ERealFormat		mFormat;
			
		
};

#endif


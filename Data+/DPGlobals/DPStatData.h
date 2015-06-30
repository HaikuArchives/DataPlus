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
#ifndef _DPStatData
#define _DPStatData

#include "DPData.h"
#include "WMFunctBase.h"
#include <AppKit.h>
#include <InterfaceKit.h>

enum ELineType {
	line_regression,
	line_identity
};

enum ELineStyle {
	style_solid,
	style_dotted,
	style_dashed
};

enum EMarkStatus {
	mkstat_none,
	mkstat_mark,
	mkstat_suppress,
	mkstat_delete
};

class DPStatData : public BHandler {

	public :
	
		DPStatData(DPData* theData);
		
	virtual
		~DPStatData();
		
	rgb_color
		ColorForValue(float val);
		
	const DPData* 
		Data() const {return mData;}
		
	virtual rgb_color
		DataColor() const {return mDataColor;}
	
	void
		EnableColorBlend(int16 dataFld);
		
	virtual void
		MarkData(BMessage* theMesg) = 0;
		
	EMarkStatus
		MarkStatus() const {return mMarkStatus;}
		
	virtual void
		MessageReceived(BMessage* theMesg);
		
	virtual void
		Recalculate() = 0;
		
	void
		RestorePen(BView* theView);
		
	void
		SetDataColor(rgb_color dataColor) {mDataColor = dataColor;}

	void
		SetLineStyle(ELineStyle style) {mLineStyle = style;}
		
	void
		SetLineType(ELineType type) {mLineType = type;}
		
	void
		SetMarkStatus(EMarkStatus status) {mMarkStatus = status;}

	void
		SetMaxColor(rgb_color maxColor) {mMaxColor = maxColor; CalcColorBlend();};
		
	void
		SetMinColor(rgb_color minColor) {mMinColor = minColor; CalcColorBlend();};
		
	void
		SetUpPen(BView* theView);
		
	virtual void
		UserSelections() = 0;
		
	protected :	// Methods
	
	void
		CalcColorBlend();
		
	virtual void
		MakePatternForFunct();
		
	protected:	// Data
	
	DPData*		mData;
	rgb_color	mDataColor;
	rgb_color	mMinColor;
	rgb_color	mMaxColor;
	float			mMinVal;
	float 		mMaxVal;
	float			mColorSpan;
	float			mRedSlope;	
	float			mGreenSlope;	
	float			mBlueSlope;	
	EMarkStatus	mMarkStatus;
	ELineType	mLineType;
	ELineStyle	mLineStyle;
	bool			mBlendColors;
};	
	
#endif


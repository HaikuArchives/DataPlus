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
#ifndef _DRRegrData
#define _DRRegrData

#include "DPStatData.h"
#include "WSLinearRegr.h"
#include "WGStdGraph.h"

enum ERegrLineType {
	rLine_none,
	rLine_regression,
	rLine_identity,
	rLine_ptTopt
};

class DRRegrData : public DPStatData {

	public :
	
		DRRegrData(DPData* theData);
		
	virtual
		~DRRegrData();
		
	ERegrLineType
		LineType() const {return mLineType;}
		
	void
		MarkData(BMessage* theMesg);
		
	void
		MessageReceived(BMessage* theMesg);
		
	virtual WMFunctBase*
		PlotFunction();
		
	EPlotMark
		PlotMark() const {return mPlotMark;}
		
	virtual void
		Recalculate();
		
	const WSLinearRegr*
		RegressionData() const {return (const WSLinearRegr*)&mRegression;}
		
	void
		SetLineType(ERegrLineType theLine) {mLineType = theLine;}
		
	void
		SetPlotMark(EPlotMark theMark) {mPlotMark = theMark;}
		
	void
		SwapAxes();
		
	virtual void
		UserSelections();
		
	protected :	// Methods
	
	virtual void
		MakePatternForFunct();
		
	protected:	// Data
	
//	WSRegressionDisp*	mDisplay;
	WSLinearRegr		mRegression;
	EPlotMark			mPlotMark;
	ERegrLineType		mLineType;
};	
	
#endif


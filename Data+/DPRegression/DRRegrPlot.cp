/* ------------------------------------------------------------------ 

	Title: DRRegrPlot

	Description:  The plotting view for Data+

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		19 August, 1999
			Created
		
------------------------------------------------------------------ */ 

#include "DRRegrPlot.h"
#include "DPData.h"
#include "DPRegression.h"
#include "DRRegrDisplay.h"
#include "DPGlobals.h"
#include "DPDataItem.h"
//#include "DPMarkWindow.h"
#include "UGraph_Util.h"
#include "WColorControl.h"
#include "Cross.h"
#include <Cursor.h>

// -------------------------------------------------------------------
// *  A class for drawing an identity line...           
// -------------------------------------------------------------------
class IdentityLine : public WMFunctBase {

	public:
		IdentityLine();
		
	virtual float
		CalcYFromX(float xVal) {mMathErr = 0; return xVal;}
};

IdentityLine::IdentityLine()
{
}
// -------------------------------------------------------------------
// *  Constructors & Destructor             
// -------------------------------------------------------------------

DRRegrPlot::DRRegrPlot(BRect frame,  
					  		  const char* title,
					  		  uint32 mode,
					  		  uint32 flags):WGStdGraph(frame,title,mode,flags)
{
	SelectEnable(true);
	mSelect = false;
	mCrossCurs = new BCursor(& Cross);
}
 
		
DRRegrPlot::~DRRegrPlot()
{
	delete mCrossCurs;
}		
// -------------------------------------------------------------------
// *  Public Methods             
// -------------------------------------------------------------------
// -------------------------------------------------------------------
void
DRRegrPlot::RefreshPlot()
{
}
// -------------------------------------------------------------------
void
DRRegrPlot::Rescale()
{
	DPRegression* theApp = cast_as(be_app,DPRegression);
	int32 theSet = 0;
	DPData* firstData = (DPData*)theApp->DataSet(theSet++);
	DPData* theData = firstData;
	float maxX;
	float minX;
	float maxY;
	float minY;
	bool firstRecord = true;
	while(theData != NULL) {
		float x,y;
		for(int32 n = 0; n < theData->NumRecords(); n++) {
			if(theData->FindRecord(n) 
			&& theData->XValue(x) 
			&& theData->YValue(y)
			&& theData->CurrentRec()->DataStatus() < data_delete) {
				if(firstRecord) {
					maxX = x;
					minX = x;
					maxY = y;
					minY = y;
					firstRecord = false;
					
				} else {
					if(x > maxX) {
						maxX = x;
					} else if(x < minX) {
						minX = x;
					}
					if(y > maxY) {
						maxY = y;
					} else if(y < minY) {
						minY = y;
					}
				}
			}
		}
		theData = (DPData*)theApp->DataSet(theSet++);
	}
	// Only do if there was data, and only use first set's names
	if(firstData != NULL) {
		UGraph_Util::AutoScale(mXAxis,minX,maxX);
		UGraph_Util::AutoScale(mYAxis,minY,maxY);
		DPDataType* theType = firstData->FieldType(firstData->XField());
		const char* label = NULL;
		if(theType != NULL) {
			label = theType->FieldName();
			if(label != NULL) {
				SetXLabel(label);
			}
		}
		theType = firstData->FieldType(firstData->YField());
		if(theType != NULL) {
			label = theType->FieldName();
			if(label != NULL) {
				SetYLabel(label);
			}
		}
		ForceReDraw();
	}
}
// -------------------------------------------------------------------
// *  Protected Methods             
// -------------------------------------------------------------------
void
DRRegrPlot::Draw(BRect frame)
{
	WGStdGraph::Draw(frame);
	PlotTo(1E10,1E10);	// Plot out of bounds
	DPRegression* theApp = cast_as(be_app,DPRegression);
	int32 theSet = 0;
	DRRegrData* theRData = (DRRegrData*)theApp->DisplayData(theSet++);
	while(theRData != NULL) {
		DPData* theData = (DPData*)theRData->Data();
		SetPlotColor(theRData->DataColor());
		SetHighColor(theRData->DataColor());	
		SetPlotMark(theRData->PlotMark());
		float x,y;
		for(int32 n = 0; n < theData->NumRecords(); n++) {
			if(theData->FindRecord(n) && theData->XValue(x) && theData->YValue(y)) {
				switch (theData->CurrentRec()->DataStatus()) {
				
					case data_normal :
						// If a Z-axis, set up colors
						if(theRData->LineType() == rLine_ptTopt) {
							PlotTo(x,y);
						}
						PlotPoint(x,y);
						break;
						
					case data_marked :
						SetPlotColor(theData->CurrentRec()->MarkColor());
						PlotPoint(x,y);
						SetPlotColor(theRData->DataColor());
						break;
						
					case data_suppress :
						SetPlotMark(plotMark_spec);
						PlotPoint(x,y);
						SetPlotMark(theRData->PlotMark());
						break;
						
					default :
						;	// Deleted stuff falls here
				}	// Of switch(
			}
		}
		theRData->SetUpPen(this);	// Set line pattern, if any
		SetHighColor(theRData->DataColor());	
		if(theRData->LineType() == rLine_regression) {
			PlotFunct(theRData->PlotFunction());
			
		} else if(theRData->LineType() == rLine_identity) {
			IdentityLine theLine;
			PlotFunct(&theLine);
		}
		SetHighColor(color_black);
		theRData->RestorePen(this);	// Restore normal pen
		theRData = (DRRegrData*)theApp->DisplayData(theSet++);
	}
	SetHighColor(color_black);
}
// -------------------------------------------------------------------
void
DRRegrPlot::MouseDown(BPoint thePoint)
{
	key_info theKeys;
	mSelect = false;
	if(get_key_info(&theKeys) == B_OK) {
		mSelect = B_SHIFT_KEY & theKeys.modifiers;
	} 
	// Change cursor if selecting
	if(mSelect) {
		be_app->SetCursor(mCrossCurs);
	}
	WGStdGraph::MouseDown(thePoint);
}
// -------------------------------------------------------------------
void
DRRegrPlot::MouseMoved(BPoint thePt, uint32 transit, const BMessage* theMesg)
{
	 // Call base clas for odometer tracking
 	WGStdGraph::MouseMoved(thePt,transit,theMesg);

}
// -------------------------------------------------------------------
void
DRRegrPlot::MouseUp(BPoint thePt)
{
	WGStdGraph::MouseUp(thePt);	// Pass through for drag-resize
	be_app->SetCursor(B_CURSOR_SYSTEM_DEFAULT);	// Maui
	DPDataItem* theItem;
	
	if(mSelect && mSelectRect.Width() > 10 && mSelectRect.Height() > 10) {
		mSelect = false;
		BMessage* theMesg = new BMessage(mesg_mark);
		int32 numSelected = 0;
		float xHi, xLo, yHi, yLo;
		PosnToValue(mSelectRect,xHi,xLo,yHi,yLo);
		DPRegression* theApp = cast_as(be_app,DPRegression);
		int32 theSet = 0;
		DPData* theData = (DPData*)theApp->DataSet(theSet++);
		while(theData != NULL) {
			int32 recNum = 0; 
			while(theData->FindRecord(recNum)) {
				float xVal, yVal;
				if(theData->XValue(xVal)
					&& theData->YValue(yVal)
					&& xVal >= xLo 
					&& xVal <= xHi
					&& yVal >= yLo
					&& yVal <= yHi) {
					// A selected record
					theItem = new DPDataItem(theData,recNum);
					BMessage itemMesg;
					theItem->Archive(&itemMesg);
					theMesg->AddMessage("DPDataItem",&itemMesg);
					delete theItem;
					numSelected++;
				}
				recNum++;
			}
			theData = (DPData*)theApp->DataSet(theSet++);
		}
		if(numSelected == 0) {
			BAlert* theAlert = new BAlert("","No Data Items Were Selected.","OK");
			theAlert->Go();
			
		} else {	// Create new data selection window
			status_t status = be_roster->Launch(data_plus_marker_sig,theMesg);

		}	
		delete theMesg;
		
	} else {	// If not for selection
		// Autoscale, if appropriate
		if(mSelectRect.Width() > 50 && mSelectRect.Height() > 50) {
			ReScaleTo(mSelectRect);
		}
	}
}
// -------------------------------------------------------------------
	

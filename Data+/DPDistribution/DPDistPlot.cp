/* ------------------------------------------------------------------ 

	Title: DPDistPlot

	Description:  The distribution plotting view for Data+

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		14 April, 2000
			Created
		
------------------------------------------------------------------ */ 

#include "DPDistPlot.h"
#include "DPDistribution.h"
#include "DPData.h"
#include "DPGlobals.h"
#include "DPDataItem.h"
#include "DPDistFit.h"
#include "UGraph_Util.h"
#include "WColorControl.h"
#include "WMProbits.h"
#include "Cross.h"
#include <Cursor.h>

// -------------------------------------------------------------------
// *  Constructors & Destructor             
// -------------------------------------------------------------------

DPDistPlot::DPDistPlot(BRect frame,  
					  		  const char* title,
					  		  uint32 mode,
					  		  uint32 flags):WGStdGraph(frame,title,mode,flags)
{
	SelectEnable(true);
	mSelect = false;
	mCrossCurs = new BCursor(& Cross);
}
 
		
DPDistPlot::~DPDistPlot()
{
	delete mCrossCurs;
}		
// -------------------------------------------------------------------
// *  Public Methods             
// -------------------------------------------------------------------
// -------------------------------------------------------------------
void
DPDistPlot::RefreshPlot()
{
}
// -------------------------------------------------------------------
void
DPDistPlot::Rescale()
{
	return; //***************
	DPDistribution* theApp = cast_as(be_app,DPDistribution);
	int32 theSet = 0;
	DPData* firstData = (DPData*)theApp->Data(theSet++);	//????
	DPData* theData = firstData;
	float maxX = -FLT_MAX;
	float minX = FLT_MAX;
	float maxY = -FLT_MAX;
	float minY = FLT_MAX;
	while(theData != NULL) {
		float x,y;
		for(int32 n = 0; n < theData->NumRecords(); n++) {
			if(theData->FindRecord(n) 
			&& theData->XValue(x) 
			&& theData->YValue(y)
			&& theData->CurrentRec()->DataStatus() < data_delete) {
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
		theData = (DPData*)theApp->DataSet(theSet++);
	}
	// Only do if there was data, and only use first set's names
	if(firstData != NULL) {
		UGraph_Util::AutoScale(mXAxis,minX,maxX);
		UGraph_Util::AutoScale(mYAxis,minY,maxY);
		DPDataType* theType = firstData->FieldType(firstData->XField());
		if(theType != NULL) {
			const char* label = theType->FieldName();
			if(label != NULL) {
				SetXLabel(label);
			}
		}
		theType = firstData->FieldType(firstData->YField());
		if(theType != NULL) {
			const char* label = theType->FieldName();
			if(label != NULL) {
				SetYLabel(label);
			}
		}
		// SetYLabel("Number");
		ForceReDraw();
	}
}
// -------------------------------------------------------------------
// *  Protected Methods             
// -------------------------------------------------------------------
void
DPDistPlot::Draw(BRect frame)
{
	WGStdGraph::Draw(frame);
	DPDistribution* theApp = cast_as(be_app,DPDistribution);
	
	switch(theApp->DistType()) {
	
		case distType_distribution :
			DrawDistribution();
			break;
			
		case distType_percentile :
			DrawPercentile();
			break;
		
		case distType_probit :
			DrawProbit();
			break;
		
		default :
			;	// No default case
	}

}
// -------------------------------------------------------------------
void
DPDistPlot::DrawDistribution()
{
	DPDistribution* theApp = cast_as(be_app,DPDistribution);
	int32 setNum = 0;
	SetViewColor(255,255,255,255);	// Transparent
	SetDrawingMode(B_OP_BLEND);
	const DPDistData* theData = theApp->DataSet(setNum++);
	while(theData != NULL) {
		for(int32 gpNum = 0; gpNum < theData->NumGroups(); gpNum++) {
			const DistGroup* theGroup = theData->GroupInfo(gpNum);
			if(theGroup->Points() > 0
			&& theGroup->Beginning() < mXAxis->Max()
			&& theGroup->End() > mXAxis->Min()) {
				BRect theRect;
				float val = theGroup->Beginning();
				if(val < mXAxis->Min()) val = mXAxis->Min() ;
				mXAxis->ValToXPosn(val,theRect.left);
				val = theGroup->End();
				if(val > mXAxis->Max()) val = mXAxis->Max() ;
				mXAxis->ValToXPosn(val,theRect.right);
				mYAxis->ValToYPosn(0,theRect.bottom);
				int32 cumPts = 0;	// Cumulative points
				// Draw one group at a time
				for(int32 setNum = 0; setNum < theGroup->NumSets(); setNum++) {
					int32 pts;
					rgb_color setColor;
					theGroup->SetInfo(setNum,pts,setColor);
					// Substitute set color if not marked
					if(setColor.alpha == 0) {
						setColor = theData->Color();
					}
					cumPts += pts;
					if(cumPts > mYAxis->Max()) cumPts = mYAxis->Max() ;
					mYAxis->ValToYPosn(cumPts,theRect.top);
					SetHighColor(setColor);
					FillRect(theRect,B_MIXED_COLORS);
					theRect.bottom = theRect.top;
				}
				mYAxis->ValToYPosn(0,theRect.bottom);
				SetHighColor(theData->Color());
				StrokeRect(theRect);
			}
		}
		// Set and plot the fitted curve
		DPDistFit* theFit = (DPDistFit*)theData->FitFunct();
		if(theApp->IsFitted() && theFit != NULL) {
			theFit->SetFitType(distType_distribution);
			PlotFunct(theFit);
		}
		theData = theApp->DataSet(setNum++);
	}
	SetHighColor(0,0,0);
}
// -------------------------------------------------------------------
void
DPDistPlot::DrawPercentile()
{
	DPDistribution* theApp = cast_as(be_app,DPDistribution);
	int32 setNum = 0;
	DPDistData* theData = (DPDistData*)theApp->DataSet(setNum++);
	PlotTo(1E10,-1E10);	// Plot out of bounds
	while(theData != NULL) {
		if(theData->DataPoints() > 0) {
			float xVal;
			float yVal;
			bool marked;
			SetHighColor(theData->Color());
			theData->Value(0,xVal,marked);
			PlotTo(1E10,-1E10);	// Plot out of bounds
			for(int32 n = 0; n < theData->DataPoints(); n++) {
				if(theData->Value(n,xVal,marked) && theData->Percentile(n,yVal)) {
					if(marked) {
						SetHighColor(theData->Data()->CurrentRec()->MarkColor());
						PlotTo(xVal,yVal);
						SetHighColor(0,0,0);
					} else {
						PlotTo(xVal,yVal);
					}
				}
			}	// Of for(int32 n..
		}	// Of if(theData->DataPoints
		// Set and plot the fitted curve
		DPDistFit* theFit = (DPDistFit*)theData->FitFunct();
		if(theApp->IsFitted() && theFit != NULL) {
			theFit->SetFitType(distType_percentile);
			PlotFunct(theFit);
		}
		theData = (DPDistData*)theApp->DataSet(setNum++);
	}
	
	SetHighColor(0,0,0);
}
// -------------------------------------------------------------------
void
DPDistPlot::DrawProbit()
{
	WMProbits probit;
	
	DPDistribution* theApp = cast_as(be_app,DPDistribution);
	int32 setNum = 0;
	DPDistData* theData = (DPDistData*)theApp->DataSet(setNum++);
	while(theData != NULL) {
		if(theData->DataPoints() > 0) {
			float xVal;
			float yVal;
			bool marked;
			SetHighColor(theData->Color());
			theData->Value(0,xVal,marked);
			PlotTo(1E10,-1E10);	// Plot out of bounds
			for(int32 n = 0; n < theData->DataPoints(); n++) {
				if(theData->Value(n,xVal,marked) && theData->Percentile(n,yVal)) {
					float pVal = probit.XformTo(yVal) - 5;
					if(!probit.MathError()) {
						if(marked) {
							SetHighColor(theData->Data()->CurrentRec()->MarkColor());
							PlotTo(xVal,pVal);
							SetHighColor(0,0,0);
						} else {
							PlotTo(xVal,pVal);
						}
					}
				}
			}	// Of for(int32 n..
		}	// Of if(theData->DataPoints
		
		// Set and plot the fitted curve
		DPDistFit* theFit = (DPDistFit*)theData->FitFunct();
		if(theApp->IsFitted() && theFit != NULL) {
			theFit->SetFitType(distType_probit);
			PlotFunct(theFit);
		}
		theData = (DPDistData*)theApp->DataSet(setNum++);
	}
	
	SetHighColor(0,0,0);
}
// -------------------------------------------------------------------
void
DPDistPlot::MouseDown(BPoint thePoint)
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
DPDistPlot::MouseMoved(BPoint thePt, uint32 transit, const BMessage* theMesg)
{
	 // Call base class for odometer tracking
 	WGStdGraph::MouseMoved(thePt,transit,theMesg);

}
// -------------------------------------------------------------------
void
DPDistPlot::MouseUp(BPoint thePt)
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
		DPDistribution* theApp = cast_as(be_app,DPDistribution);
		int32 theSet = 0;
		DPData* theData = (DPData*)theApp->Data(theSet++);
		while(theData != NULL) {
			int32 recNum = 0; 
			while(theData->FindRecord(recNum)) {
				float xVal;
				if(theData->XValue(xVal)
					&& xVal >= xLo 
					&& xVal <= xHi) {
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
			theData = (DPData*)theApp->Data(theSet++);
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
	

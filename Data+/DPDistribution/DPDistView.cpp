/* ------------------------------------------------------------------ 

	Title: DPDistView

	Description:  The master distribution plot view This is a view
		which holds the distribution data output panel, plus controls
		for changing the distribution marks and color.

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		05 June, 2000
			Revised for distribution view
		23 November, 1999
			Revised for single window look
		30 October, 1999
			Added controls
		14 August, 1999
			Created
		
------------------------------------------------------------------ */ 
#include "DPDistView.h"
#include "DPGlobals.h"

#include <SupportKit.h>
#include <AppKit.h>

const uint32	apply_change	= 'AChg';

// -------------------------------------------------------------------
// *  Constructors & Destructor             
// -------------------------------------------------------------------
DPDistView::DPDistView(DPDistData*	theDistData,
						BRect frame,
			   			const char* title,
			   			uint32 mode, 
			   			uint32 flags):BView(frame,title,mode,flags)
{
	mData = theDistData;
	mKey = theDistData->Data()->Key();	// For future linking
	SetViewColor(color_std_gray);
	mDataView = new WSDistributionView(theDistData->DistributionData());
	AddChild(mDataView);
	mColorCtrl = new WColorControl(BRect(0,0,5,5),"wcolor",NULL);
	AddChild(mColorCtrl);
	mButton = new BButton(BRect(0,0,5,5),"","Apply",new BMessage(apply_change));
	AddChild(mButton);
	ResetDisplay();
}

DPDistView::~DPDistView()
{
}
// -------------------------------------------------------------------
// *  Public Methods             
// -------------------------------------------------------------------
void
DPDistView::GetPreferredSize(float* width, float* height)
{
	BView::GetPreferredSize(width,height);
}
// -------------------------------------------------------------------
void
DPDistView::Draw(BRect frame)
{
	// StrokeRect(Bounds());
}
// -------------------------------------------------------------------
void
DPDistView::MessageReceived(BMessage* theMesg)
{
	BMessage* dispMesg;
	
	switch (theMesg->what) {
	
		case apply_change :
			dispMesg = new BMessage(set_display);
			dispMesg->AddInt64(disp_set_name,mKey);
			dispMesg->AddInt32(mark_col_name,mColorCtrl->Value());
			be_app->PostMessage(dispMesg);
			delete dispMesg;
			break;
			
		default :
			BView::MessageReceived(theMesg);
	}
}		
// -------------------------------------------------------------------
void
DPDistView::ResetDisplay()
{
/*
	const DPData* theData = mData->Data();
	BString theString;
	theString.Append(theData->SetName());
	theData->MakeTitleString(&theString,false);
	mDataView->SetTitle(theString.String());
	DPDataType* theType = theData->FieldType(theData->XField());
	if(theType != NULL) {
		mDataView->SetXLabel(theType->FieldName());
	}
	theType = theData->FieldType(theData->YField());
	if(theType != NULL) {
		mDataView->SetYLabel(theType->FieldName());
	}
	mDataView->Invalidate();
*/
}
// -------------------------------------------------------------------
void
DPDistView::ResizeToPreferred()
{
	const float ctrl_margin = 3;
	float vWd, vHt;
	mDataView->ResizeToPreferred();
	mDataView->GetPreferredSize(&vWd,&vHt);
	float cWd, cHt;
	mColorCtrl->ResizeToPreferred();
	mColorCtrl->GetPreferredSize(&cWd,&cHt);
	float bWd, bHt;
	mButton->ResizeToPreferred();
	mButton->GetPreferredSize(&bWd,&bHt);
	float viewWd = vWd + ctrl_margin*2;
	float viewHt = vHt + cHt + ctrl_margin*3;
	ResizeTo(viewWd,viewHt);
	mDataView->MoveTo(ctrl_margin,ctrl_margin);
	// The color control is thicker, so we want to center it
	float center = viewHt - (ctrl_margin + cHt/2);
	mColorCtrl->MoveTo(ctrl_margin,center - cHt/2);
	mButton->MoveTo(viewWd - (bWd + ctrl_margin),center - bHt/2);
}
// -------------------------------------------------------------------
// *  Protected Methods             
// -------------------------------------------------------------------
void
DPDistView::AttachedToWindow()
{
	mButton->SetTarget(this);
}
// -------------------------------------------------------------------

		

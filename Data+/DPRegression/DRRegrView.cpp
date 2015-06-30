/* ------------------------------------------------------------------ 

	Title: DRRegrView

	Description:  The master regression plot view This is a view
		which holds the regression data output panel, plus controls
		for changing the plot marks and color.

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		23 November, 1999
			Revised for single window look
		30 October, 1999
			Added controls
		14 August, 1999
			Created
		
------------------------------------------------------------------ */ 
#include "DRRegrView.h"
#include "DPGlobals.h"

#include <SupportKit.h>

const uint32	apply_change	= 'AChg';

const char no_line_label[8]		= "No Line";
const char regr_line_label[16]	= "Regression";
const char ident_line_label[16]	= "Identity";
const char ptp_line_label[16]		= "Point To Point";

// -------------------------------------------------------------------
// *  Constructors & Destructor             
// -------------------------------------------------------------------
DRRegrView::DRRegrView(DRRegrData*	theRData,
						BRect frame,
			   			const char* title,
			   			uint32 mode, 
			   			uint32 flags):BView(frame,title,mode,flags)
{
	mData = theRData;
	mKey = theRData->Data()->Key();	// For future linking
	SetViewColor(color_std_gray);
	mDataView = new WSRegressionView(theRData->RegressionData());
	AddChild(mDataView);
	mPlotMark = new WGPlotMarkMenu(BRect(0,0,5,5),"plotmark",NULL);
	AddChild(mPlotMark);
	mColorCtrl = new WColorControl(BRect(0,0,5,5),"wcolor",NULL);
	AddChild(mColorCtrl);
	BMenu* theMenu = new BPopUpMenu(regr_line_label);
	theMenu->AddItem(new BMenuItem(no_line_label,NULL));
	BMenuItem* rItem = new BMenuItem(regr_line_label,NULL);
	theMenu->AddItem(rItem);
	theMenu->AddItem(new BMenuItem(ident_line_label,NULL));
	theMenu->AddItem(new BMenuItem(ptp_line_label,NULL));
	mLineSelect = new BMenuField(BRect(0,0,0,0),NULL,NULL,theMenu);
	mLineSelect->ResizeToPreferred();
	mLineSelect->SetDivider(0);
	rItem->SetMarked(true);
	AddChild(mLineSelect);
	mButton = new BButton(BRect(0,0,5,5),"","Apply",new BMessage(apply_change));
	AddChild(mButton);
	ResetDisplay();
}

DRRegrView::~DRRegrView()
{
}
// -------------------------------------------------------------------
// *  Public Methods             
// -------------------------------------------------------------------
void
DRRegrView::GetPreferredSize(float* width, float* height)
{
	BView::GetPreferredSize(width,height);
}
// -------------------------------------------------------------------
void
DRRegrView::Draw(BRect frame)
{
	StrokeRect(Bounds());
}
// -------------------------------------------------------------------
void
DRRegrView::MessageReceived(BMessage* theMesg)
{
	BMessage* dispMesg;
	int16 itemChecked;
	BMenu* theMenu;
	
	switch (theMesg->what) {
	
		case apply_change :
			dispMesg = new BMessage(set_display);
			dispMesg->AddInt64(disp_set_name,mKey);
			dispMesg->AddInt16(disp_mark_name,mPlotMark->PlotMark());
			dispMesg->AddInt32(mark_col_name,mColorCtrl->Value());
			theMenu = mLineSelect->Menu();
			itemChecked = theMenu->IndexOf(theMenu->FindMarked());
			dispMesg->AddInt16(disp_line_name,itemChecked);
			be_app->PostMessage(dispMesg);
			delete dispMesg;
			break;
			
		default :
			BView::MessageReceived(theMesg);
	}
}		
// -------------------------------------------------------------------
void
DRRegrView::ResetDisplay()
{
	const DPData* theData = mData->Data();
	BString theString;
	theString.Append(theData->SetName());
//	theData->MakeTitleString(&theString,false);
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
}
// -------------------------------------------------------------------
void
DRRegrView::ResizeToPreferred()
{
	const float ctrl_margin = 3;
	float vWd, vHt;
	mDataView->ResizeToPreferred();
	mDataView->GetPreferredSize(&vWd,&vHt);
	float mWd,mHt;
	mPlotMark->ResizeToPreferred();
	mPlotMark->GetPreferredSize(&mWd,&mHt);
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
	mPlotMark->MoveTo(ctrl_margin,center - mHt/2);
	mColorCtrl->MoveTo(mPlotMark->Frame().right + ctrl_margin,center - cHt/2);
	mLineSelect->MoveTo(mColorCtrl->Frame().right + ctrl_margin*2,center - mHt/2 - 1);
	mButton->MoveTo(viewWd - (bWd + ctrl_margin),center - bHt/2);
}
// -------------------------------------------------------------------
// *  Protected Methods             
// -------------------------------------------------------------------
void
DRRegrView::AttachedToWindow()
{
	mButton->SetTarget(this);
}
// -------------------------------------------------------------------

		

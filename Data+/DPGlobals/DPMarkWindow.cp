/* ------------------------------------------------------------------ 

	Title: DPMarkWindow

	Description:  The window where selected data may be marked

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		07 October, 1999
			Created
		
------------------------------------------------------------------ */ 

#include "DPMarkWindow.h"
#include "DPDataRec.h"
#include "WColorControl.h"
#include "WU_AppUtils.h"
#include "DPDataItem.h"
#include "DPGlobals.h"

const uint32 apply_mesg = 'Aply';
// -------------------------------------------------------------------
// *  Constructors & Destructor             
// -------------------------------------------------------------------
DPMarkWindow::DPMarkWindow(BMessage* theData,
									BRect frame, 
					 				const char* name,
					 				window_look look,
					 				window_feel feel,
					 				uint32 flags):BWindow(frame,name,look,feel,flags)
{
	// Make a holding frame for all other views
	BView* theView = new BView(Bounds(),"",B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
	theView->SetViewColor(color_std_gray);
	AddChild(theView);
	
	mListView = new DPDataList(theData);
	theView->AddChild(mListView);
	
	// Create controls
	mMark = new BRadioButton(BRect(0,0,0,0),"","Mark",NULL);
	theView->AddChild(mMark);
	mSuppress = new BRadioButton(BRect(0,0,0,0),"","Suppress",NULL);
	theView->AddChild(mSuppress);
	mDelete = new BRadioButton(BRect(0,0,0,0),"","Delete",NULL);
	theView->AddChild(mDelete);
	mColorCtrl = new WColorControl(BRect(0,0,5,5));
	theView->AddChild(mColorCtrl);
	mButton = new BButton(BRect(0,0,0,0),"","Apply",new BMessage(apply_mesg));
	theView->AddChild(mButton);
	
	// Resize & move elements
	const float	ctrl_margin = 5;
	const float sview_ht = 18;
	mListView->ResizeToPreferred();
	mMark->ResizeToPreferred();
	mDelete->ResizeToPreferred();
	mSuppress->ResizeToPreferred();
	mColorCtrl->ResizeToPreferred();
	mButton->ResizeToPreferred();
	float lWidth, lHeight;
	mListView->GetPreferredSize(&lWidth,&lHeight);
	// Add a string view for titling
	BRect svRect(ctrl_margin,0,ctrl_margin + lWidth, ctrl_margin + sview_ht);
	BStringView* theStrView = new BStringView(svRect,"","Record                        X-val  Y-val");
	theView->AddChild(theStrView);
	
	float rWidth, rHeight;
	mSuppress->GetPreferredSize(&rWidth,&rHeight);
	float cWidth,cHeight;
	mColorCtrl->GetPreferredSize(&cWidth,&cHeight);
	float wWidth = lWidth + ctrl_margin*2;
	float wHeight = sview_ht+ lHeight + rHeight*3 + cHeight + ctrl_margin*7;
	ResizeTo(wWidth,wHeight);	// 	Resize window - view follows
	mListView->MoveTo(ctrl_margin,theStrView->Frame().bottom + ctrl_margin);
	mMark->MoveTo(ctrl_margin,mListView->Frame().bottom + ctrl_margin);
	mSuppress->MoveTo(ctrl_margin,mMark->Frame().bottom + ctrl_margin);
	mDelete->MoveTo(ctrl_margin,mSuppress->Frame().bottom + ctrl_margin);
	mColorCtrl->MoveTo(ctrl_margin,mDelete->Frame().bottom + ctrl_margin/2);
	float vCenter = (mColorCtrl->Frame().top + mListView->Frame().bottom)/2;
	float hCenter = (mSuppress->Frame().right + Bounds().right)/2;
	mButton->MoveTo(hCenter - mButton->Bounds().Width()/2,vCenter - mButton->Bounds().Height()/2);
	mMark->SetValue(1);
	Show();
}
// -------------------------------------------------------------------
// *  Public Methods             
// -------------------------------------------------------------------
void
DPMarkWindow::MessageReceived(BMessage* theMesg)
{
	BMessage* keyMesg;
	int32 itemNum = 0;
	DPDataItem* theItem;
	EDataStatus theStatus = data_marked;
	
	switch(theMesg->what) {
	
		case apply_mesg :
			keyMesg = new BMessage(mesg_mark);
			// Add the mark color
			keyMesg->AddInt32(mark_col_name,mColorCtrl->Value());
			// Add the mark status
			if(mSuppress->Value() > 0) {
				theStatus = data_suppress;
			} else if(mDelete->Value() > 0) {
				theStatus = data_delete;
			}
			keyMesg->AddInt16(status_name,theStatus);
			// Add the record keys
			do {
				theItem = (DPDataItem*)mListView->ItemAt(itemNum++);
				if(theItem != NULL) {
					keyMesg->AddInt64(keys_names,theItem->ItemKey());
				}
			} while (theItem != NULL) ;
			//Broadcast to all plotting apps
			::BroadcastToClients(data_plus_mark_client_sig,keyMesg);
			Quit();
			break;
		
		default :
			BWindow::MessageReceived(theMesg);
	}
}	
// -------------------------------------------------------------------
// *  Protected Methods             
// -------------------------------------------------------------------

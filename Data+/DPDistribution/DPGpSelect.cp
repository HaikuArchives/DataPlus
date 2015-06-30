/* ------------------------------------------------------------------ 

	Title: DPGpSelect

	Description:  Dialog for selecting group size

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		19 July, 2000
			Created
		
------------------------------------------------------------------ */ 

#include "DPGpSelect.h"
#include "URealFmt_Utils.h"
#include "DPDistWindow.h"
#include <stdio.h>

const uint32 apply_mesg = 'Aply';
// -------------------------------------------------------------------
// *  Constructors & Destructor             
// -------------------------------------------------------------------
DPGpSelect::DPGpSelect(float oldSize,
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
	
	// Create controls
	const float sview_ht = 18;
	char dStr[16];
	::FormatReal(oldSize,realFmt_float,4,dStr);
	mControl = new BTextControl(BRect(0,0,100,sview_ht),"ctrl","Group Size:",dStr,NULL);
	theView->AddChild(mControl);
	mButton = new BButton(BRect(0,0,0,0),"","Apply",new BMessage(apply_mesg));
	theView->AddChild(mButton);
	
	// Resize & move elements
	const float	ctrl_margin = 5;
	mControl->ResizeToPreferred();
	float cWidth, cHeight;
	mControl->GetPreferredSize(&cWidth, &cHeight);
	mButton->ResizeToPreferred();
	float bWidth, bHeight;
	mButton->GetPreferredSize(&bWidth,&bHeight);

	ResizeTo(cWidth + ctrl_margin*2,cHeight + bHeight + ctrl_margin*3);	// 	Resize window - view follows
	mControl->MoveTo(ctrl_margin,ctrl_margin);
	mButton->MoveTo(Bounds().right - (ctrl_margin + bWidth),Bounds().bottom - (ctrl_margin + bHeight));
	Show();
}
// -------------------------------------------------------------------
// *  Public Methods             
// -------------------------------------------------------------------
void
DPGpSelect::MessageReceived(BMessage* theMesg)
{
	BMessage* keyMesg;
	float theVal = 0;
	
	switch(theMesg->what) {
	
		case apply_mesg :
			keyMesg = new BMessage(mesg_gpSize);
			if(::sscanf(mControl->Text(),"%f",&theVal) == 1
			&& theVal > 0) {
				keyMesg->AddFloat("",theVal);
				be_app->PostMessage(keyMesg);
			}
			delete keyMesg;
			Quit();
			break;
		
		default :
			BWindow::MessageReceived(theMesg);
	}
}	
// -------------------------------------------------------------------
// *  Protected Methods             
// -------------------------------------------------------------------

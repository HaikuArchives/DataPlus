/* ------------------------------------------------------------------ 

	Title: DPFldWindow

	Description:  Allows the manual selection of data fields.
	
	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		23 August, 1999
			Created
		
------------------------------------------------------------------ */ 
#include "DPFldWindow.h"
#include "DPGlobals.h"

#include <AppKit.h>

const uint32	ok_mesg 		= 'isOK';
const uint32	sel_hit		= 'sHit';
const char		x_label[8] = "X-Axis";
const char		y_label[8] = "Y-Axis";
const char		z_label[8] = "Z-Axis";
const char		no_label[8] = "None";

// -------------------------------------------------------------------
//	* CONSTRUCTORS and DESTRUCTORS
// -------------------------------------------------------------------
DPFldWindow::DPFldWindow(DPData* theData,
								 BRect frame,
						 		 char*	name,
						 		 window_type type,
						 		 uint32 flags):BWindow(frame,name,type,flags)
{
	mData = theData;
	mSelector = NULL;
	CreateWindow(frame);
	Show();
}
		
DPFldWindow::~DPFldWindow()
{
	delete mSelector;
}
// -------------------------------------------------------------------
//	* PUBLIC METHODS
// -------------------------------------------------------------------
void
DPFldWindow::MessageReceived(BMessage* theMesg)
{
	switch (theMesg->what) {
	
		case ok_mesg :
			SetFields();
			be_app->PostMessage(FLDS_RESET);
			Quit();
			break;
			
		case sel_hit :
			TestSelections();
			break;
			
		default :
			BWindow::MessageReceived(theMesg);
	}
}
// -------------------------------------------------------------------
//	* PROTECTED METHODS
// -------------------------------------------------------------------
void
DPFldWindow::CreateWindow(BRect frame)
{
	// Create space for the fields
	mNumFlds = mData->NumFields();
	mSelector = new BMenuField*[mNumFlds];
	
	// Add the basic view
	mView = new BView(Bounds(),"View",B_FOLLOW_LEFT_RIGHT || B_FOLLOW_TOP_BOTTOM,B_WILL_DRAW);
	AddChild(mView);
	
	BRect mFrame;
	BPopUpMenu* theMenu;
	// Find the longest field string
	float sMax = 0;
	float len;
	for(int n = 0; n < mData->NumFields(); n++) {
		len = mView->StringWidth(mData->FieldType(n)->FieldName());
		sMax = len > sMax ? len : sMax;
	}
	const float v_interval = 25;
	const float menu_left = 20;
	const float menu_ht = 22;
	const float menu_wd = 60;
	const float s_margin = 5;
	float fTop = 10;
	float sWidth = menu_wd + sMax + s_margin;
	// Add the items
	for(int n = 0; n < mNumFlds; n++) {
		mFrame.Set(menu_left,fTop,menu_left + sWidth,fTop + menu_ht);
		// Create the basic menu
		theMenu = new BPopUpMenu(no_label);
		theMenu->AddItem(new BMenuItem(no_label,new BMessage(sel_hit)));
		theMenu->AddItem(new BMenuItem(x_label,new BMessage(sel_hit)));
		theMenu->AddItem(new BMenuItem(y_label,new BMessage(sel_hit)));
		theMenu->AddItem(new BMenuItem(z_label,new BMessage(sel_hit)));
		mSelector[n] = new BMenuField(mFrame,"Menu",NULL,theMenu);
		mSelector[n]->SetDivider(sMax + s_margin);
		mSelector[n]->SetAlignment(B_ALIGN_RIGHT);
		mSelector[n]->SetLabel(mData->FieldType(n)->FieldName());
		mView->AddChild(mSelector[n]);
		fTop += v_interval;
	}
	
	// Add an "OK" button
	mFrame.top = fTop + 20;
	mFrame.bottom = fTop + 25;
	mFrame.left = menu_left;
	mFrame.right = menu_left + 50;
	BMessage* theMesg = new BMessage(ok_mesg);
	BButton* theButton = new BButton(mFrame,"OKButton","OK",theMesg);
	theButton->SetTarget(this);
	mView->AddChild(theButton);
	
	// Size the main view and window	
	ResizeTo(menu_left + sWidth + menu_left,mFrame.bottom + 20);
	
	// Set the field selector menus
	if(mData->XField() >= 0) {
		mSelector[mData->XField()]->Menu()->FindItem(x_label)->SetMarked(true);
	}
	if(mData->YField() >= 0) {
		mSelector[mData->YField()]->Menu()->FindItem(y_label)->SetMarked(true);
	}
	if(mData->ZField() >= 0) {
		mSelector[mData->ZField()]->Menu()->FindItem(z_label)->SetMarked(true);
	}
	
}
// -------------------------------------------------------------------
void
DPFldWindow::SetFields()
{
	int16 xFld = -1;
	int16 yFld = -1;
	int16 zFld = -1;
	
	BMenuItem* theItem;
	for(int fld = 0; fld < mNumFlds; fld++) {
		// Find checked item, if a duplicate, reset to 'none'
		BMenu* theMenu = mSelector[fld]->Menu();
		theItem = theMenu->FindItem(x_label);
		if(theItem->IsMarked()) {
			xFld = fld;
		}
		theItem = theMenu->FindItem(y_label);
		if(theItem->IsMarked()) {
			yFld = fld;
		}
		theItem = theMenu->FindItem(z_label);
		if(theItem->IsMarked()) {
			zFld = fld;
		}
	}
	mData->SetXYZFields(xFld,yFld,zFld);
}
// -------------------------------------------------------------------
void
DPFldWindow::TestSelections()
{
	bool hasX = false;
	BMenu* xMenu = NULL;
	bool hasY = false;
	BMenu* yMenu = NULL;
	bool hasZ = false;
	BMenu* zMenu = NULL;
	BMenuItem* theItem;
	for(int fld = 0; fld < mNumFlds; fld++) {
		// Find checked item, if a duplicate, reset to 'none'
		BMenu* theMenu = mSelector[fld]->Menu();
		bool reset = false;
		theItem = theMenu->FindItem(x_label);
		if(theItem->IsMarked()) {
			if(!hasX) {
				hasX = true;
				xMenu = theMenu;
			} else {
				theItem->SetMarked(false);
				reset = true;
			}
		}
		theItem = theMenu->FindItem(y_label);
		if(theItem->IsMarked()) {
			if(!hasY) {
				hasY = true;
				yMenu = theMenu;
			} else {
				theItem->SetMarked(false);
				reset = true;
			}
		}
		theItem = theMenu->FindItem(z_label);
		if(theItem->IsMarked()) {
			if(!hasZ) {
				hasZ = true;
				zMenu = theMenu;
			} else {
				theItem->SetMarked(false);
				reset = true;
			}
		}
		if(reset) {
			theItem = theMenu->FindItem(no_label);
			theItem->SetMarked(true);
		}	
	}
	// Make sure the fields are checked in x,y,z sequence
	if(hasY && !hasX) {
		BAlert* theAlert = new BAlert("Chk","An X-Item Must Be Selected If A \n Y-Item Is Selected","OK");
		theAlert->Go();
		yMenu->FindItem(no_label)->SetMarked(true);
		hasY = false;
	}
	if(hasZ && !hasY) {
		BAlert* theAlert = new BAlert("Chk","A Y-Item Must Be Selected If A \n Z-Item Is Selected","OK");
		theAlert->Go();
		zMenu->FindItem(no_label)->SetMarked(true);
	}
}
// -------------------------------------------------------------------
